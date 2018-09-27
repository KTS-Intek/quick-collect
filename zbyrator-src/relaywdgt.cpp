#include "relaywdgt.h"
#include "ui_relaywdgt.h"
#include "map-pgs/mapwidget.h"
#include "src/matilda/settloader.h"
#include "src/widgets/selectionchecker.h"
#include "src/matilda/showmesshelper4wdgt.h"
#include "src/meter/definedpollcodes.h"
#include "src/zbyrator-v2/quickpollhelper.h"


RelayWdgt::RelayWdgt(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    ReferenceWidgetClass(lDevInfo, gHelper, gSett4all, parent),
    ui(new Ui::RelayWdgt)
{
    ui->setupUi(this);
    isMapReady = false;
//    contextPbRead = hasContxBtn;
}

RelayWdgt::~RelayWdgt()
{
    delete ui;
}

QVariant RelayWdgt::getPageSett4read(bool &ok, QString &mess)
{
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




}

void RelayWdgt::onModelChanged()
{
//    QVariantList vl;

//    //    QStringList l = GuiHelper::getColNamesLedLamp().split(",");
//    //tr("Model,NI,Group,Last Exchange,Power [%],Start Power [%],NA Power [%],Tna [sec],Coordinate,Poll On/Off,Street,Memo") ;

//    //return tr("Model,Serial Number,NI,Memo,Password,On/Off,Physical values,Tariff Count,Coordinate,Meter Version")

//    const QStringList l4app = QString("ni pos pll img").split(" ");
//    const QList<int> l4appIndx = QList<int>() << 0 << 8 ; //-1


//    for(int i = 0, iMax = proxy_model->rowCount(), lMax = l4appIndx.size(); i < iMax; i++){
//        int row = proxy_model->mapToSource(proxy_model->index(i, 0)).row();
//        QVariantHash h;
//        for(int l = 0; l < lMax; l++)//службова інформація
//            h.insert(l4app.at(l), model->item(row, l4appIndx.at(l))->text());

//        const QString tltp = QString("NI: <b>%1</b>, %2<br>")
//                .arg(model->item(row, 0)->text())
//                .arg(tr("Model: %1, Code %2<br>Time: %3<br>Stat: %4, %5<br>Elapsed: %6 [sec]<br>Last Retry: %7")
//                     .arg(model->item(row, 1)->text())//model
//                     .arg(model->item(row, 2)->text())//code
//                     .arg(model->item(row, 3)->text())//time
//                     .arg(model->item(row, 4)->text())//stat
//                     .arg(model->item(row, 5)->text())//stat
//                     .arg(model->item(row, 6)->text())//elapsed
//                     .arg(model->item(row, 7)->text()));//last retr


//        h.insert("grp", model->item(row, 1)->text() + "\t" + model->item(row, 7)->text());

//        h.insert("tltp", tltp);
//        h.insert("rowid", row + 1);
//        if(h.value("pos").toString().isEmpty())//якщо нема координат, то і нема чого показувать
//            continue;
//        vl.append(h);
//    }


//    if(!lDevInfo->matildaDev.coordinatesIsDefault){
//        QVariantHash h;
//        h.insert("pos", QString("%1,%2").arg(QString::number(lDevInfo->matildaDev.coordinates.x(), 'f', 6)).arg(QString::number(lDevInfo->matildaDev.coordinates.y(), 'f', 6)) );
//        h.insert("isMatilda", true);
//        h.insert("rowid", "Z");
//        h.insert("ni", tr("Universal Communicator"));
//        h.insert("tltp", h.value("ni").toString() + tr("<br>SN: %1<br>").arg(lDevInfo->matildaDev.lastSerialNumber));
//        vl.prepend(h);
//    }


//    emit setNewDeviceModelEs(vl);

//    const int row = proxy_model->mapToSource(ui->tvWithFilter->currentIndex()).row();
//    if(row >= 0)
    //        emit showThisDeviceNIEs(model->item(row, 2)->text());
}

void RelayWdgt::meterRelayStatus(QString ni, QDateTime dtLocal, QString stts)
{
    if(ni.isEmpty())
        return;

    const int row = StandardItemModelHelper::getRowFromNI(4, ni, model);
    if(row < 0)
        return;

    model->item(row, 0)->setText(dtLocal.toString(lastDateTimeMask));
    model->item(row, 1)->setText(stts);

    TableViewHelper::selectRowWithThisCell(ui->tvTable, ni, 4);

    resizeLastTv2content();
}

void RelayWdgt::initPage()
{
    setupObjects(ui->tvTable, ui->tbFilter, ui->cbFilterMode, ui->leFilter, SETT_FILTERS_RELAYWDGT);
    StandardItemModelHelper::modelSetHorizontalHeaderItems(model, QStringList());

    ui->widget_2->setEnabled(false);
    ui->widget->setEnabled(false);

    connect(gHelper, SIGNAL(setPbWriteEnableDisable(bool)), ui->pbRead, SLOT(setDisabled(bool)));
    connect(gHelper, SIGNAL(setPbWriteEnableDisable(bool)), ui->pbLoadOff, SLOT(setDisabled(bool)));
    connect(gHelper, SIGNAL(setPbWriteEnableDisable(bool)), ui->pbLoadOn, SLOT(setDisabled(bool)));

    ui->pbRead->setDisabled(gHelper->managerEnDisBttn.pbWriteDis);
    ui->pbLoadOff->setDisabled(gHelper->managerEnDisBttn.pbWriteDis);
    ui->pbLoadOn->setDisabled(gHelper->managerEnDisBttn.pbWriteDis);

    emit onReloadAllMeters();

    SelectionChecker *tmr = new SelectionChecker(this);
    tmr->setWatchTable(ui->tvTable, ui->widget_2);
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

        //        connect(w, SIGNAL(showThisDev(QString)), this, SLOT(showThisDev(QString)) );
        //        connect(w, SIGNAL(addDevice(QString))  , this, SLOT(addDevice(QString))   );
        //        connect(w, SIGNAL(updateModel4ls())    , this, SLOT(onModelChanged())     );
        //        connect(w, SIGNAL(removeDevice(QString)), this, SLOT(removeDevice(QString)));
        //        connect(w, SIGNAL(moveDevice(QVariantHash)), this, SLOT(moveDevice(QVariantHash)));
        //        connect(w, SIGNAL(addDeviceStreet(QVariantHash)), this, SLOT(addDeviceStreet(QVariantHash)));

//        connect(ui->tbShowMap, SIGNAL(toggled(bool)), w, SLOT(isParentWidgetVisible(bool)) );

        QTimer::singleShot(11, this, SLOT(onModelChanged()) );
    }

    emit showMapEs(gHelper->getLastLang());
}

void RelayWdgt::on_tvTable_customContextMenuRequested(const QPoint &pos)
{
    gHelper->createCustomMenu(pos, ui->tvTable, (GuiHelper::ShowReset|GuiHelper::ShowExport|GuiHelper::ShowOnlyCopy), CLBRD_SMPL_PRXTBL, ShowMessHelper4wdgt::matildaFileName(windowTitle()));
}


void RelayWdgt::onPbReadAll_clicked()
{
    doRelayOperation(TableViewHelper::getRowsText(ui->tvTable, 4), RELAY_READ);
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
    doRelayOperation(TableViewHelper::selectedRowText(ui->tvTable, 4), operation);
}

void RelayWdgt::doRelayOperation(const QStringList &listni, const quint8 &operation)
{
    if(gHelper->managerEnDisBttn.pbReadDis)
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
    StandardItemModelHelper::clearCells(listni, 4,QList<int>() << 0 << 1, model);
    gHelper->updateSettDateMaskAndDotPos();
    emit setLastPageId(accessibleName());
    lastDateTimeMask = gHelper->dateMask + " hh:mm:ss";
    emit command4dev(POLL_CODE_RELAY_OPERATIONS, map);

}
