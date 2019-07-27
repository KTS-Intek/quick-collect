#include "relaywdgt.h"
#include "ui_relaywdgt.h"

///[!] map-widget
#include "map-pgs/mapwidget.h"


///[!] widgets-shared
#include "src/widgets/selectionchecker.h"
#include "src/meter/relaystatehelper.h"


///[!] zbyrator-shared
#include "src/zbyrator-v2/quickpollhelper.h"


///[!] guisett-shared-core
#include "src/nongui/meterstatehelper4gui.h"
#include "src/nongui/showmesshelpercore.h"
#include "src/nongui/settloader.h"


#include "definedpollcodes.h"


RelayWdgt::RelayWdgt(GuiHelper *gHelper, QWidget *parent) :
    ReferenceWidgetClass(gHelper,  parent),
    ui(new Ui::RelayWdgt)
{
    ui->setupUi(this);
    isMapReady = false;
    ui->pbLoadOff->setEnabled(false);
    ui->pbLoadOn->setEnabled(false);
    ui->pbLoadOff_2->setEnabled(false);
    ui->pbLoadOn_2->setEnabled(false);
    ui->pbRead->setEnabled(false);
//    contextPbRead = hasContxBtn;
}

RelayWdgt::~RelayWdgt()
{
    delete ui;
}

QVariant RelayWdgt::getPageSett4read(bool &ok, QString &mess)
{
    //Ucon read electricity meter list framed ext -> read relay state table,
    //relay state table -
    //NI
    //
    Q_UNUSED(ok);
    Q_UNUSED(mess);
    QTimer::singleShot(1, this, SLOT(onPbReadAll_clicked()));
    return QVariantHash();
}

void RelayWdgt::clearPage()
{
    model->clear();
    ui->tbFilter->setEnabled(false);
    ui->tbShowList->animateClick();

}

void RelayWdgt::setPageSett(const MyListStringList &listRows, const QVariantMap &col2data, const QStringList &headerH, const QStringList &header, const bool &hasHeader)
{
//    const QString currentNI

    const QString currNi = headerH.contains("NI") ? TableViewHelper::getCellValueOfcurrentRow(ui->tvTable, headerH.indexOf("NI")) : "";

    StandardItemModelHelper::append2model(listRows, col2data, headerH, header, hasHeader, model);

    if(!currNi.isEmpty())
        TableViewHelper::selectRowWithThisCell(ui->tvTable, currNi, headerH.indexOf("NI"));

    ui->widget->setDisabled(listRows.isEmpty());

    setHasDataFromRemoteDevice();

    emit resizeTv2content(ui->tvTable);


    ui->tbFilter->setEnabled(true);

}

void RelayWdgt::onModelChanged()
{
    QVariantList vl;

//    QStringList l = GuiHelper::getColNamesLedLampV2().split(",");
    //tr("Model,NI,Group,Last Exchange,Power [%],Start Power [%],NA Power [%],Tna [sec],Coordinate,Poll On/Off,Street,Memo") ;

    //Time Relay Meter S/N NI Memo Coordinate
    QStringList l4app = QString("ni pos grp pll img").split(" ");
    QList<int> l4appIndx;
    l4appIndx << 5 << 7 << 2 << 1 ; //-1


    const QList<int> l4tltp = QList<int>() << 0 << 2 << 3 << 4 << 6;

    for(int i = 0, iMax = proxy_model->rowCount(), lMax = l4appIndx.size(), jmax = l4tltp.size(); i < iMax; i++){
        int row = proxy_model->mapToSource(proxy_model->index(i, 0)).row();
        QVariantHash h;
        for(int l = 0; l < lMax; l++)//службова інформація
            h.insert(l4app.at(l), model->item(row, l4appIndx.at(l))->text());

        if(h.value("pos").toString().isEmpty())//якщо нема координат, то і нема чого показувать
            continue;

        QStringList tltp;
        tltp.append(tr("NI: <b>%1</b>, Relay: <b>%2</b>").arg(model->item(row, 5)->text()).arg(model->item(row, 1)->text()));


        for(int j = 0; j < jmax; j++){
            const QString s = model->item(row, l4tltp.at(j))->text();
            if(s.isEmpty() || s == "?" || s == "!")
                continue;



            tltp.append(QString("%1: <b>%2</b>").arg(model->horizontalHeaderItem(l4tltp.at(j))->text()).arg(s));

        }




        h.insert("tltp", tltp.join("<br>") + "<br>");
        h.insert("rowid", row + 1);
        const QString img = model->item(row, 0)->data(Qt::UserRole + 23).toString();
        if(!img.isEmpty())
            h.insert("img", "qrc" + img );//m.at(icoCol)->setData(icoList.at(i), Qt::UserRole + 23);

        vl.append(h);
    }


    if(!lDevInfo->matildaDev.coordinatesIsDefault){
        QVariantHash h;
        h.insert("pos", QString("%1,%2").arg(QString::number(lDevInfo->matildaDev.coordinates.x(), 'f', 6)).arg(QString::number(lDevInfo->matildaDev.coordinates.y(), 'f', 6)) );
        h.insert("isMatilda", true);
        h.insert("rowid", "Z");
        h.insert("ni", tr("Universal Communicator"));
        h.insert("tltp", h.value("ni").toString() + tr("<br>S/N: %1<br>").arg(lDevInfo->matildaDev.lastSerialNumber));
        vl.prepend(h);
    }


    emit setNewDeviceModelEs(vl);

    int row = proxy_model->mapToSource(ui->tvTable->currentIndex()).row();
    if(row >= 0)
        emit showThisDeviceNIEs(model->item(row, 5)->text());
}

void RelayWdgt::meterRelayStatus(QString ni, QDateTime dtLocal, quint8 mainstts, quint8 secondarystts)
{
    if(ni.isEmpty())
        return;

    const int row = StandardItemModelHelper::getRowFromNI(5, ni, model);
    if(row < 0)
        return;

    model->item(row, 0)->setText(dtLocal.toString(lastDateTimeMask));
    model->item(row, 1)->setText(RelayStateHelper::getRelayStatusHuman(mainstts));
    model->item(row, 2)->setText(RelayStateHelper::getRelayStatusHuman(secondarystts));

    model->item(row, 1)->setIcon(QIcon(MeterStateHelper4gui::getRelayIcostr4status(mainstts)));
    model->item(row, 2)->setIcon(QIcon(MeterStateHelper4gui::getRelayIcostr4status(secondarystts)));

    TableViewHelper::selectRowWithThisCell(ui->tvTable, ni, 5);

    resizeLastTv2content();
}

void RelayWdgt::showThisDev(QString ni)
{
    TableViewHelper::selectRowWithThisCell(lastTv, ni, 5);

}

void RelayWdgt::showContextMenu4thisDev(QString ni)
{
    showThisDev(ni);
    emit request2showContextMenuAnimated();

}

void RelayWdgt::showThisDevInSource(QString ni)
{
    showThisDev(ni);
    ui->tbShowList->animateClick();
}

void RelayWdgt::onWdgtLock(bool disable)
{
    sendActLock(disable, !ui->pbRead->isEnabled());
}

void RelayWdgt::onButtonLock(bool disable)
{
    sendActLock(!ui->widget_2->isEnabled(), disable);
}

void RelayWdgt::sendActLock(const bool &isWdgtDisabled, const bool &isButtonDisabled)
{
    emit lockActions((isWdgtDisabled || isButtonDisabled));

}

void RelayWdgt::initPage()
{
    setupObjects(ui->horizontalLayout_61, ui->tvTable, ui->tbFilter, ui->cbFilterMode, ui->leFilter, SETT_FILTERS_RELAYWDGT);
    connect(this, SIGNAL(openContextMenu(QPoint)), this, SLOT(on_tvTable_customContextMenuRequested(QPoint)));
    StandardItemModelHelper::setModelHorizontalHeaderItems(model, QStringList());

    ui->widget_2->setEnabled(false);
    ui->widget->setEnabled(false);



    connect(this, SIGNAL(lockButtons(bool)), ui->pbRead, SLOT(setDisabled(bool)));
    connect(this, SIGNAL(lockButtons(bool)), ui->pbLoadOff, SLOT(setDisabled(bool)));
    connect(this, SIGNAL(lockButtons(bool)), ui->pbLoadOn, SLOT(setDisabled(bool)));

    connect(this, SIGNAL(lockButtons(bool)), ui->pbLoadOff_2, SLOT(setDisabled(bool)));
    connect(this, SIGNAL(lockButtons(bool)), ui->pbLoadOn_2, SLOT(setDisabled(bool)));

    lastDateTimeMask = gHelper->getDateTimeMask();

    emit onReloadAllMeters();

    SelectionChecker *tmr = new SelectionChecker(this);
    tmr->setWatchTable(ui->tvTable, ui->widget_2);
    tmr->setTextLbl4disp(ui->label_2, tr("Selected [%1]"), tr("Selected"));

    connect(tmr, &SelectionChecker::setWdgtDisable, this, &RelayWdgt::onWdgtLock);// ui->widget_3, &QWidget::setVisible);

//    readDefCommandOnUpdate();
}

void RelayWdgt::on_tbShowList_clicked()
{
    ui->tbShowList->setChecked(true);
    ui->stackedWidget->setCurrentIndex(0);
    ui->tbShowMap->setChecked(false);
}

void RelayWdgt::on_tbShowMap_clicked()
{
    ui->tbShowList->setChecked(false);
    ui->stackedWidget->setCurrentIndex(1);
    ui->tbShowMap->setChecked(true);

    if(!isMapReady){
        isMapReady = true;

        QTimer *tmrModelChanged = new QTimer(this);
        tmrModelChanged->setInterval(555);
        tmrModelChanged->setSingleShot(true);


        proxy_model->enableModelChangedSignal();
        connect(proxy_model, SIGNAL(onModelChanged()), tmrModelChanged, SLOT(start()) );
        connect(tmrModelChanged, SIGNAL(timeout()), this, SLOT(onModelChanged()) );

        MapWidget *w = new MapWidget(true);
        ui->vlQmlMap->addWidget(w);

        connect(this, SIGNAL(showMapEs(QString))  , w, SLOT(showMap(QString)) );

        connect(this, SIGNAL(setNewDeviceModelEs(QVariantList)), w, SIGNAL(setNewDeviceModel(QVariantList)) );
        connect(this, SIGNAL(showThisDeviceNIEs(QString))      , w, SIGNAL(showThisDeviceNI(QString)) );
        //        connect(this, SIGNAL(showThisCoordinatesEs(QString))   , w, SIGNAL(showThisCoordinates(QString)) );

                connect(w, SIGNAL(showThisDev(QString)), this, SLOT(showThisDev(QString)) );
                connect(w, SIGNAL(showContextMenu4thisDev(QString)), this, SLOT(showContextMenu4thisDev(QString)));
                connect(w, SIGNAL(showThisDevInSource(QString)), this, SLOT(showThisDevInSource(QString)));

        //        connect(w, SIGNAL(addDevice(QString))  , this, SLOT(addDevice(QString))   );
        //        connect(w, SIGNAL(updateModel4ls())    , this, SLOT(onModelChanged())     );
        //        connect(w, SIGNAL(removeDevice(QString)), this, SLOT(removeDevice(QString)));
        //        connect(w, SIGNAL(moveDevice(QVariantHash)), this, SLOT(moveDevice(QVariantHash)));
        //        connect(w, SIGNAL(addDeviceStreet(QVariantHash)), this, SLOT(addDeviceStreet(QVariantHash)));

//        connect(ui->tbShowMap, SIGNAL(toggled(bool)), w, SLOT(isParentWidgetVisible(bool)) );

        QTimer::singleShot(11, this, SLOT(onModelChanged()) );
    }

    emit showMapEs(gHelper->guiSett->currLang);
}

void RelayWdgt::on_tvTable_customContextMenuRequested(const QPoint &pos)
{
    gHelper->createCustomMenu(pos, ui->tvTable, (GuiHelper::ShowReset|GuiHelper::ShowExport|GuiHelper::ShowOnlyCopy), CLBRD_SMPL_PRXTBL, ShowMessHelperCore::matildaFileName(windowTitle()), getRelayActions());
}


void RelayWdgt::onPbReadAll_clicked()
{
    doRelayOperation(TableViewHelper::getRowsText(ui->tvTable, 5), RELAY_READ);
}

void RelayWdgt::on_pbRead_clicked()
{
    doRelayOperationSelected(RELAY_READ);
}

void RelayWdgt::on_pbLoadOn_clicked()
{
    doRelayOperationSelected(RELAY_LOAD_ON);

}

void RelayWdgt::on_pbLoadOff_clicked()
{
    doRelayOperationSelected(RELAY_LOAD_OFF);
}

void RelayWdgt::doRelayOperationSelected(const quint8 &operation)
{
    doRelayOperation(TableViewHelper::getSelectedRowsText(ui->tvTable, 5), operation);
}

void RelayWdgt::doRelayOperation(const QStringList &listni, const quint8 &operation)
{
    if(gHelper->managerEnDisBttn.pbWriteDis)
        return;

    if(listni.isEmpty()){
        gHelper->showMessSlot(tr("no meters"));
        return;
    }

    QString mess;
    const QVariantMap map = QuickPollHelper::createPollMap4relay(listni,operation, mess);

    if(map.isEmpty())
        return;

    ui->tvTable->clearSelection();
    StandardItemModelHelper::clearCells(listni, 5,QList<int>() << 0 << 1 << 2, model);

    emit setLastPageId(accessibleName());
    lastDateTimeMask = gHelper->getDateTimeMask();
    emit command4dev(POLL_CODE_RELAY_OPERATIONS, map);

}

QList<QAction *> RelayWdgt::getRelayActions()
{
    QList<QAction*> la;
    la.append(createActionFromButton(ui->pbRead));

    la.append(createActionFromButton(ui->pbLoadOff));
    la.append(createActionFromButton(ui->pbLoadOff_2));

    la.append(createActionFromButton(ui->pbLoadOn));
    la.append(createActionFromButton(ui->pbLoadOn_2));

    const bool enbl = (ui->pbRead->isEnabled() && ui->widget_2->isEnabled());
    for(int i = 0, imax = la.size(); i < imax; i++){
        connect(this, SIGNAL(lockActions(bool)), la.at(i), SLOT(setDisabled(bool)));
        la.at(i)->setEnabled(enbl);
    }
    return la;
}



void RelayWdgt::on_pbLoadOff_2_clicked()
{
    doRelayOperationSelected(RELAY2_LOAD_OFF);

}

void RelayWdgt::on_pbLoadOn_2_clicked()
{
    doRelayOperationSelected(RELAY2_LOAD_ON);

}
