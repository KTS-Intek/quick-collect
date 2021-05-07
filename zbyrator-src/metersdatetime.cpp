#include "metersdatetime.h"



///[!] zbyrator-shared
#include "src/zbyrator-v2/quickpollhelper.h"

///[!] type-converter
#include "src/base/convertatype.h"


#include "definedpollcodes.h"


//---------------------------------------------------------------------------------------

MetersDateTime::MetersDateTime(GuiHelper *gHelper, QWidget *parent) :
    ReferenceWidgetClassGui(gHelper,  parent)
{
    hidePbAdd();

    createTopWidget();
    setHasReadButton(true);
}

//---------------------------------------------------------------------------------------

void MetersDateTime::replaceHeaderRoles4map(QStringList &heaaderroles, int &colKey, int &colPos)
{
    if(heaaderroles.size() > 7){
#ifdef HAS_MAP_WIDGETS
        heaaderroles.replace(0, QString::number(BaseMapMarkersModel::itmmarkertxt));//row number
        heaaderroles.replace(5, QString::number(BaseMapMarkersModel::itmkeyvaluetxt));//NI
        heaaderroles.replace(8, QString::number(BaseMapMarkersModel::coordinatevar));//gps
#endif
    }
    colKey = 4; //ni
    colPos = 7; //gps
}

//---------------------------------------------------------------------------------------

void MetersDateTime::updateMapGroupingSettings()
{//    return tr("Computer,Meter time,Meter,S/N,NI,Memo,DST settings,Coordinate").split(",");

    const QStringList lk = StandardItemModelHelper::getColText(2, -1, model);

    QVariantMap mapdefaultgrouppingsett;
    QVariantMap mapdata;

    for(int i = 0, imax = lk.size(); i < imax; i++){
        const QString model = lk.at(i);

        mapdata.insert(model,   QCryptographicHash::hash(model.toLocal8Bit(), QCryptographicHash::Md5).right(3).toHex().toUpper());//model 2 color RRGGBB

        qDebug() << "mapdata" << model << mapdata.value(model);
    }



    mapdefaultgrouppingsett.insert("cols", QString::number(4).split(" "));//model
    mapdefaultgrouppingsett.insert("data", mapdata);
    mapdefaultgrouppingsett.insert("en", true);


    emit setDefaultDataFilterSettings(mapdefaultgrouppingsett, QString("MetersDateTime"));



}

//---------------------------------------------------------------------------------------

QString MetersDateTime::updatePageContent(QString &errorStr)
{
    //quick collect sends wmeter and emeter at the same time
    return gHelper->ucDeviceTreeW->getUCEMeterSettings(accessibleName(), errorStr) ? accessibleName() : "";

}

//---------------------------------------------------------------------------------------

MTableFullHouse MetersDateTime::fromUCPollDeviceSettingsList(const QList<UCPollDeviceSettings> &settings)
{
    MTableFullHouse outt;

    //    MNPrintableOut ntable;

    for(int i = 0, imax = settings.size(); i < imax; i++){
        const UCPollDeviceSettings onerow = settings.at(i);


        // tr("Computer,Meter time,Meter,S/N,NI,Memo,DST settings,Coordinate").split(",");
        QStringList list;

        list.append(""); //pc time
        list.append(""); //meter time

        QStringList meterl;
        meterl.append(onerow.model);
        meterl.append(onerow.vrsn);
        list.append(meterl.join(", "));

        list.append(onerow.sn);
        list.append(onerow.ni);
        list.append(onerow.memo);

        list.append(""); //meter dst


        list.append(ConvertAtype::coordinateToStr(onerow.coordinate));


        outt.table.append(list);

    }
    return outt;
}

//---------------------------------------------------------------------------------------

QStringList MetersDateTime::getHeader()
{
    return tr("Computer,Meter time,Meter,S/N,NI,Memo,DST settings,Coordinate").split(",");
}

//---------------------------------------------------------------------------------------

QStringList MetersDateTime::getVisibleNIs()
{
    return TableViewHelper::getSelectedRowsText(lastTv, 4);
}

//---------------------------------------------------------------------------------------

QStringList MetersDateTime::getSelectedNIs()
{
    return TableViewHelper::getSelectedRowsText(lastTv, 4);
}

//---------------------------------------------------------------------------------------

void MetersDateTime::clearPage()
{

}

//---------------------------------------------------------------------------------------

//void MetersDateTime::setPageSett(const MyListStringList &listRows, const QVariantMap &col2data, const QStringList &headerH, const QStringList &header, const bool &hasHeader)
//{

//    const QString currNi = headerH.contains("NI") ? TableViewHelper::getCellValueOfcurrentRow(ui->tvTable, headerH.indexOf("NI")) : "";

//    StandardItemModelHelper::append2model(listRows, col2data, headerH, header, hasHeader, model);

//    if(!currNi.isEmpty())
//        TableViewHelper::selectRowWithThisCell(ui->tvTable, currNi, headerH.indexOf("NI"));

//    ui->widget->setDisabled(listRows.isEmpty());

//    setHasDataFromRemoteDevice();
//    ui->tbFilter->setEnabled(!header.isEmpty());

//    emit resizeTv2content(ui->tvTable);
//}


//---------------------------------------------------------------------------------------

void MetersDateTime::meterDateTimeDstStatus(QString ni, QDateTime dtLocal, QString stts)
{
    if(ni.isEmpty())
        return;

    const int row = StandardItemModelHelper::getRowFromNI(4, ni, model);
    if(row < 0)
        return;

    stts.append("\n\n");
    const QStringList l = stts.split("\n");

    LastMetersDateTimeDst onedev;
    onedev.msecpc = dtLocal.toMSecsSinceEpoch();
    onedev.dtmeterstr = l.at(0);
    onedev.dstmeter = l.at(1);

    lDateTimeDst.checkednis.removeOne(ni);
    lDateTimeDst.dttable.insert(ni, onedev);

    emit stopTmrRelyaStatusTmr();
    updateDateTimeDst();

    TableViewHelper::selectRowWithThisCell(lastTv, ni, 4);

}

void MetersDateTime::updateDateTimeDst()
{

    QTime time;
    time.start();
    QStringList nichanged;

    const int imax = model->rowCount();
    int i = 0;


    for( ; i < imax; i++){

        const QString ni = model->item(i, 5)->text();
        if(lDateTimeDst.checkednis.contains(ni))
            continue;

        lDateTimeDst.checkednis.append(ni);

        const LastMetersDateTimeDst relay = lDateTimeDst.dttable.value(ni);

//        return tr("Time,Main Relay,Second Relay,Meter,S/N,NI,Memo,Coordinate").split(",");


        model->item(i,0)->setText( (relay.msecpc > 0) ?
                                       QDateTime::fromMSecsSinceEpoch(relay.msecpc).toLocalTime().toString(lastDateTimeMask) :
                                       "");
        model->item(i,1)->setText(relay.dtmeterstr);
        model->item(i,6)->setText(relay.dstmeter);






        if(time.elapsed() > 333)
            break;
    }

    if(i < imax)
        emit updateRelayStatusTmr();
    else
        resizeLastTv2content();


    if(!nichanged.isEmpty()){
        const QModelIndex lastindx = TableViewHelper::selectTheseCells(nichanged, 2, false, lastTv);

        if(lastindx.isValid())
            lastTv->scrollTo(lastindx);
    }
    qDebug() << "MetersDateTime updateRelayStatus " << imax << lDateTimeDst.checkednis.size();
}

//---------------------------------------------------------------------------------------

void MetersDateTime::onStartOperation(bool selectedOnly, quint8 operation)
{
    startOperation(selectedOnly ? getSelectedNIs() : getVisibleNIs(), operation);
}

//---------------------------------------------------------------------------------------

void MetersDateTime::onUCEMeterSettingsChanged(UCEMeterSettings settings)
{
    Q_UNUSED(settings);
    //do not do anything , wait until wmeter settins are here
}

//---------------------------------------------------------------------------------------

void MetersDateTime::onUCWMeterSettingsChanged(UCWMeterSettings settings)
{

    QList<UCPollDeviceSettings> baseSettingsL;
    const UCEMeterSettings emeterSettings = gHelper->ucDeviceTreeW->getCachedUCEMeterSettings();
    for(int i = 0, imax = emeterSettings.eMeterContainer.size(); i < imax; i++)
        baseSettingsL.append(emeterSettings.eMeterContainer.at(i).baseSettings);


    for(int i = 0, imax = settings.wMeterContainer.size(); i < imax; i++)
        baseSettingsL.append(settings.wMeterContainer.at(i).baseSettings);

    const MTableFullHouse outt = fromUCPollDeviceSettingsList(baseSettingsL);
    setTableData4amodelExt4(outt, getHeader(), settings.validator);

    buttonsWidget->setEnabled(!outt.table.isEmpty());
    //    getLastRelayStateSmart(settings.validator);
    emit updateRelayStatusTmr();

}

//---------------------------------------------------------------------------------------

void MetersDateTime::initPage()
{


    createModelsV2("MetersDateTime", true);
    connectMessageSignal();


    QTimer *tmrrelay = new QTimer(this);
    tmrrelay->setInterval(333);
    tmrrelay->setSingleShot(true);
    connect(this, SIGNAL(stopTmrRelyaStatusTmr()), tmrrelay, SLOT(stop()));
    connect(this, SIGNAL(updateRelayStatusTmr()), tmrrelay, SLOT(start()));

    connect(tmrrelay, SIGNAL(timeout()), this, SLOT(updateDateTimeDst()));


    connect(this, &MetersDateTime::openContextMenu, this, &MetersDateTime::onTvTableCustomContextMenuRequested);
    connect(lastTv, &QTableView::customContextMenuRequested, this, &MetersDateTime::onTvTableCustomContextMenuRequested);

    clearPage();

    connect(gHelper->ucDeviceTreeW, &UCDeviceTreeWatcher::onUCWMeterSettingsChanged, this, &MetersDateTime::onUCWMeterSettingsChanged);
    connect(gHelper->ucDeviceTreeW, &UCDeviceTreeWatcher::onUCEMeterSettingsChanged, this, &MetersDateTime::onUCEMeterSettingsChanged);



    //    QString m; all data must be in the memory
        if(gHelper->ucDeviceTreeW->getCachedUCEMeterSettings().validator.dtlastupdate.isValid()){
             onUCEMeterSettingsChanged(gHelper->ucDeviceTreeW->getCachedUCEMeterSettings());
             onUCWMeterSettingsChanged(gHelper->ucDeviceTreeW->getCachedUCWMeterSettings());


        }
    //    else
    //        updatePageContent(m);



    connect(this, &MetersDateTime::lockButtons, buttonsWidget, &MetersDateTimeButtonsWdgt::lockButtons);
    connect(buttonsWidget, &MetersDateTimeButtonsWdgt::onStartOperation, this ,&MetersDateTime::onStartOperation);

    buttonsWidget->createSelectionChecker(lastTv);

    QTimer::singleShot(11, this, SLOT(createMapWidgetLaterReadOnly()));
    emit onPageCanReceiveData();




}

//---------------------------------------------------------------------------------------

void MetersDateTime::onTvTableCustomContextMenuRequested(const QPoint &pos)
{
    QList<QAction*> la = getDateTimeActions();

    createCustomMenu4tv(pos,
                        (GuiHelper::ShowReset|GuiHelper::ShowAdd2Filter|GuiHelper::ShowExport|GuiHelper::ShowOnlyCopy),
                        getCopyPastTag(), la);



}

//---------------------------------------------------------------------------------------

void MetersDateTime::onPbReadAll_clicked()
{
    onStartOperation(false, POLL_CODE_READ_DATE_TIME_DST);
}

//---------------------------------------------------------------------------------------

void MetersDateTime::createTopWidget()
{
    buttonsWidget = new MetersDateTimeButtonsWdgt(this);
    getTopLayout()->insertWidget(0, buttonsWidget);
    buttonsWidget->setEnabled(false);
}

//---------------------------------------------------------------------------------------

void MetersDateTime::startOperation(const QStringList &listni, const quint8 &operation)
{
    if(gHelper->managerEnDisBttn.pbReadDis)
        return;

    if(listni.isEmpty()){
        gHelper->showMessageSlot(tr("no meters"));
        return;
    }

    QString mess;
    const QVariantMap map = QuickPollHelper::createPollMap4relay(listni, 0, mess);

    if(map.isEmpty())
        return;

    lastTv->clearSelection();
    StandardItemModelHelper::clearCells(listni, 4,QList<int>() << 0 << 1 << 6, model);
    gHelper->updateSettDateMaskAndDotPos();
    emit setLastPageId(accessibleName());
    lastDateTimeMask = gHelper->getDateTimeMask();
    emit command4dev(operation, map);
}

//---------------------------------------------------------------------------------------

QList<QAction *> MetersDateTime::getDateTimeActions()
{
    QList<QAction*> la;
    foreach (QPushButton *pb, buttonsWidget->gimmeYourButtons()) {
        la.append(createActionFromButton(pb));
    }

    const bool enbl = buttonsWidget->gimmeYourEnabled();

    for(int i = 0, imax = la.size(); i < imax; i++){
        connect(buttonsWidget, SIGNAL(lockActions(bool)), la.at(i), SLOT(setDisabled(bool)));
        la.at(i)->setEnabled(enbl);
    }
    return la;
}

//---------------------------------------------------------------------------------------
