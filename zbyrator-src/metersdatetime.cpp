#include "metersdatetime.h"
#include "ui_metersdatetime.h"
#include "map-pgs/mapwidget.h"
#include "src/nongui/settloader.h"
#include "src/widgets/selectionchecker.h"
#include "src/nongui/showmesshelpercore.h"
#include "src/meter/definedpollcodes.h"
#include "src/zbyrator-v2/quickpollhelper.h"


MetersDateTime::MetersDateTime(GuiHelper *gHelper, QWidget *parent) :
    ReferenceWidgetClass(gHelper,  parent),
    ui(new Ui::MetersDateTime)
{
    ui->setupUi(this);
    isMapReady = false;

    ui->pbCorrectionAll->setEnabled(false);
    ui->pbRead->setEnabled(false);
    ui->pbWrite->setEnabled(false);

}

MetersDateTime::~MetersDateTime()
{
    delete ui;
}

QVariant MetersDateTime::getPageSett4read(bool &ok, QString &mess)
{
    Q_UNUSED(ok);
    Q_UNUSED(mess);
    QTimer::singleShot(1, this, SLOT(onPbReadAll_clicked()));
    return QVariantHash();
}

void MetersDateTime::clearPage()
{

}

void MetersDateTime::setPageSett(const MyListStringList &listRows, const QVariantMap &col2data, const QStringList &headerH, const QStringList &header, const bool &hasHeader)
{

    const QString currNi = headerH.contains("NI") ? TableViewHelper::getCellValueOfcurrentRow(ui->tvTable, headerH.indexOf("NI")) : "";

    StandardItemModelHelper::append2model(listRows, col2data, headerH, header, hasHeader, model);

    if(!currNi.isEmpty())
        TableViewHelper::selectRowWithThisCell(ui->tvTable, currNi, headerH.indexOf("NI"));

    ui->widget->setDisabled(listRows.isEmpty());

    setHasDataFromRemoteDevice();
    ui->tbFilter->setEnabled(!header.isEmpty());

    emit resizeTv2content(ui->tvTable);
}


void MetersDateTime::onModelChanged()
{

    const QList<int> hiddenCols = QList<int>();
    const QList<int> rowsList = StandardItemModelHelper::getSourceRows(WidgetsHelper::getRowsListTo(proxy_model->rowCount()), proxy_model);//  ;
    int valCounter;///unused

    const QStringList headerlist = StandardItemModelHelper::modelHeaderListWithRowID(model, hiddenCols);

    QStringList heaaderroles;
    for(int i = 0, imax = headerlist.size(); i < imax; i++)
        heaaderroles.append(0);

    if(!heaaderroles.isEmpty()){
       heaaderroles.replace(0, QString::number(BaseMapMarkersModel::itmmarkertxt));

       heaaderroles.replace(5, QString::number(BaseMapMarkersModel::itmkeyvaluetxt));//NI
       heaaderroles.replace(8, QString::number(BaseMapMarkersModel::coordinatevar));
   //    heaaderroles.replace(3, QString::number(BaseMapMarkersModel::itmvalueTxt));
    }

    emit setModelHeaderDataRoles(heaaderroles.join("\n"));
    const MPrintTableOut out = StandardItemModelHelper::getModelAsVector(model, proxy_model, hiddenCols, rowsList, true, valCounter);

    emit setTableDataExt(out, headerlist, 5);

    const int row = proxy_model->mapToSource(ui->tvTable->currentIndex()).row();
    if(row < 0)
        return;
    emit showThisDeviceKeyValue(model->item(row, 5)->text());


}

void MetersDateTime::meterDateTimeDstStatus(QString ni, QDateTime dtLocal, QString stts)
{
    if(ni.isEmpty())
        return;

    const int row = StandardItemModelHelper::getRowFromNI(4, ni, model);
    if(row < 0)
        return;

    stts.append("\n\n");
    const QStringList l = stts.split("\n");

    model->item(row, 0)->setText(dtLocal.toString(lastDateTimeMask));
    model->item(row, 1)->setText(l.at(0));
    model->item(row, 6)->setText(l.at(1));

    TableViewHelper::selectRowWithThisCell(ui->tvTable, ni, 4);

    resizeLastTv2content();
}

void MetersDateTime::showThisDev(QString ni)
{
    TableViewHelper::selectRowWithThisCell(lastTv, ni, 4);

}

void MetersDateTime::showContextMenu4thisDev(QString ni)
{
    showThisDev(ni);
    emit request2showContextMenuAnimated();
}

void MetersDateTime::showThisDevInSource(QString ni)
{
    showThisDev(ni);
    ui->tbShowList->animateClick();
}

void MetersDateTime::onWdgtLock(bool disable)
{
    sendActLock(disable, !ui->pbRead->isEnabled());
}

void MetersDateTime::onButtonLock(bool disable)
{
    sendActLock(!ui->widget_2->isEnabled(), disable);
}

void MetersDateTime::sendActLock(const bool &isWdgtDisabled, const bool &isButtonDisabled)
{
    emit lockActions((isWdgtDisabled || isButtonDisabled));
}

void MetersDateTime::initPage()
{

    setupObjects(ui->horizontalLayout_61, ui->tvTable, ui->tbFilter, ui->cbFilterMode, ui->leFilter, SETT_FILTERS_METERDATETIME);
    connect(this, SIGNAL(openContextMenu(QPoint)), this, SLOT(on_tvTable_customContextMenuRequested(QPoint)));

    StandardItemModelHelper::setModelHorizontalHeaderItems(model, QStringList());

    ui->widget_2->setEnabled(false);
    ui->widget->setEnabled(false);

    connect(this, SIGNAL(lockButtons(bool)), ui->pbRead, SLOT(setDisabled(bool)));
    connect(this, SIGNAL(lockButtons(bool)), ui->pbWrite, SLOT(setDisabled(bool)));
    connect(this, SIGNAL(lockButtons(bool)), ui->pbCorrectionAll, SLOT(setDisabled(bool)));

    SelectionChecker *tmr = new SelectionChecker(this);
    tmr->setWatchTable(ui->tvTable, ui->widget_2);
    tmr->setTextLbl4disp(ui->label_2, tr("Selected [%1]"), tr("Selected"));

    ui->widget_3->setVisible(false);
    connect(tmr, &SelectionChecker::setWdgtEnable, ui->widget_3, &QWidget::setVisible);
    connect(tmr, &SelectionChecker::setWdgtDisable, this, &MetersDateTime::onWdgtLock);// ui->widget_3, &QWidget::setVisible);

    emit onReloadAllMeters();


}

void MetersDateTime::on_tbShowList_clicked()
{
    ui->tbShowList->setChecked(true);
    ui->stackedWidget->setCurrentIndex(0);
    ui->tbShowMap->setChecked(false);
}

void MetersDateTime::on_tbShowMap_clicked()
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

        connect(this, &MetersDateTime::setTableDataExt, w, &MapWidget::setTableDataExt);
        connect(this, &MetersDateTime::setModelHeaderDataRoles, w, &MapWidget::setModelHeaderDataRoles);
        connect(this, &MetersDateTime::setDefaultDataFilterSettings, w, &MapWidget::setDefaultDataFilterSettings);
        connect(this, &MetersDateTime::showThisDeviceKeyValue, w, &MapWidget::showThisDeviceKeyValue);



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

                connect(w, &MapWidget::mapIsReady, this, &MetersDateTime::onModelChanged);

    }

    if(true){
//        const QStringList lk = getAvRelayStatuses();

        QVariantMap mapdefaultgrouppingsett;
        QVariantMap mapdata;

//        for(int i = 0, imax = lk.size(); i < imax; i++){
//            const QString model = lk.at(i);

//            mapdata.insert(model,   QCryptographicHash::hash(model.toLocal8Bit(), QCryptographicHash::Md5).right(3).toHex().toUpper());//model 2 color RRGGBB

//            qDebug() << "mapdata" << model << mapdata.value(model);
//        }



        mapdefaultgrouppingsett.insert("cols", QString::number(1).split(" "));
        mapdefaultgrouppingsett.insert("data", mapdata);
        mapdefaultgrouppingsett.insert("en", true);


        emit setDefaultDataFilterSettings(mapdefaultgrouppingsett, QString("relaysV0"));
    }


    emit showMapEs(gHelper->guiSett->currLang);
}

void MetersDateTime::on_tvTable_customContextMenuRequested(const QPoint &pos)
{
    gHelper->createCustomMenu(pos, ui->tvTable, (GuiHelper::ShowReset|GuiHelper::ShowExport|GuiHelper::ShowOnlyCopy), CLBRD_SMPL_PRXTBL, ShowMessHelperCore::matildaFileName(windowTitle()), getDateTimeActions());

}

void MetersDateTime::onPbReadAll_clicked()
{
    startOperation(TableViewHelper::getRowsText(ui->tvTable, 4), POLL_CODE_READ_DATE_TIME_DST);
}

void MetersDateTime::on_pbCorrectionAll_clicked()
{
    startOperation(TableViewHelper::getRowsText(ui->tvTable, 4), POLL_CODE_WRITE_DATE_TIME);
}

void MetersDateTime::on_pbRead_clicked()
{
    startOperation(TableViewHelper::getSelectedRowsText(ui->tvTable, 4), POLL_CODE_READ_DATE_TIME_DST);
}

void MetersDateTime::on_pbWrite_clicked()
{
    startOperation(TableViewHelper::getSelectedRowsText(ui->tvTable, 4), POLL_CODE_WRITE_DATE_TIME);

}

void MetersDateTime::startOperation(const QStringList &listni, const quint8 &operation)
{
    if(gHelper->managerEnDisBttn.pbReadDis)
        return;

    if(listni.isEmpty()){
        gHelper->showMessSlot(tr("no meters"));
        return;
    }

    QString mess;
    const QVariantMap map = QuickPollHelper::createPollMap4relay(listni, 0, mess);

    if(map.isEmpty())
        return;

    ui->tvTable->clearSelection();
    StandardItemModelHelper::clearCells(listni, 4,QList<int>() << 0 << 1 << 6, model);
    gHelper->updateSettDateMaskAndDotPos();
    emit setLastPageId(accessibleName());
    lastDateTimeMask = gHelper->dateMask + " hh:mm:ss";
    emit command4dev(operation, map);
}

QList<QAction *> MetersDateTime::getDateTimeActions()
{
    QList<QAction*> la;
    la.append(createActionFromButton(ui->pbCorrectionAll));

    la.append(createActionFromButton(ui->pbRead));
      la.append(createActionFromButton(ui->pbWrite));



      const bool enbl = (ui->pbRead->isEnabled() && ui->widget_2->isEnabled());
        for(int i = 0, imax = la.size(); i < imax; i++){
            connect(this, SIGNAL(lockActions(bool)), la.at(i), SLOT(setDisabled(bool)));
            la.at(i)->setEnabled(enbl);
        }
        return la;
}
