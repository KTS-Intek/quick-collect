#include "relaywdgt.h"


///[!] widgets-shared
#include "src/meter/relaystatehelper.h"


///[!] zbyrator-shared
#include "src/zbyrator-v2/quickpollhelper.h"


///[!] guisett-shared-core
//#include "src/nongui/meterstatehelper4gui.h"



///[!] type-converter
#include "src/base/convertatype.h"


#include "definedpollcodes.h"


//-----------------------------------------------------------------------------

RelayWdgt::RelayWdgt(GuiHelper *gHelper, QWidget *parent) :
    ReferenceWidgetClassGui(gHelper,  parent)
{
    hidePbAdd();

    createTopWidget();
    setHasReadButton(true);
    //    contextPbRead = hasContxBtn;
}

//-----------------------------------------------------------------------------

void RelayWdgt::replaceHeaderRoles4map(QStringList &heaaderroles, int &colKey, int &colPos)
{
    if(heaaderroles.size() > 7){
#ifdef HAS_MAP_WIDGETS
        heaaderroles.replace(0, QString::number(BaseMapMarkersModel::itmmarkertxt));//row number
        heaaderroles.replace(6, QString::number(BaseMapMarkersModel::itmkeyvaluetxt));//NI
        heaaderroles.replace(8, QString::number(BaseMapMarkersModel::coordinatevar));//gps
#endif
    }
    colKey = 5; //ni
    colPos = 7; //gps
}

//-----------------------------------------------------------------------------

void RelayWdgt::updateMapGroupingSettings()
{

    const QStringList lk = getAvRelayStatuses();

    QVariantMap mapdefaultgrouppingsett;
    QVariantMap mapdata;

    for(int i = 0, imax = lk.size(); i < imax; i++){
        const QString model = lk.at(i);

        mapdata.insert(model,   QCryptographicHash::hash(model.toLocal8Bit(), QCryptographicHash::Md5).right(3).toHex().toUpper());//model 2 color RRGGBB

        qDebug() << "mapdata" << model << mapdata.value(model);
    }



    mapdefaultgrouppingsett.insert("cols", QString::number(1).split(" "));
    mapdefaultgrouppingsett.insert("data", mapdata);
    mapdefaultgrouppingsett.insert("en", true);



    emit setDefaultDataFilterSettings(mapdefaultgrouppingsett, QString("metertype-%1-relay").arg(int(UC_METER_ELECTRICITY)));
}

//-----------------------------------------------------------------------------

QString RelayWdgt::updatePageContent(QString &errorStr)
{
    return gHelper->ucDeviceTreeW->getUCEMeterSettings(accessibleName(), errorStr) ? accessibleName() : "";

}

//-----------------------------------------------------------------------------

MTableFullHouse RelayWdgt::fromUCEMeterSettings(const UCEMeterSettings &settings)
{
    MTableFullHouse outt;

//    MNPrintableOut ntable;

    for(int i = 0, imax = settings.eMeterContainer.size(); i < imax; i++){
        const UCEMeterSettingsOneRow onerow = settings.eMeterContainer.at(i);


        //r("Time,Main Relay,Second Relay,Meter,S/N,NI,Memo,Coordinate").split(",");
        QStringList list;

        list.append(""); //time
        list.append(""); //main
        list.append("");//secondary


        QStringList meterl;
        meterl.append(onerow.baseSettings.model);
        meterl.append(onerow.baseSettings.vrsn);
        list.append(meterl.join(", "));

        list.append(onerow.baseSettings.sn);
        list.append(onerow.baseSettings.ni);
        list.append(onerow.baseSettings.memo);

        list.append(ConvertAtype::coordinateToStr(onerow.baseSettings.coordinate));


        outt.table.append(list);

    }
    return outt;
}

//-----------------------------------------------------------------------------

QStringList RelayWdgt::getHeader()
{
    return tr("Time,Main Relay,Second Relay,Meter,S/N,NI,Memo,Coordinate").split(",");

}



//-----------------------------------------------------------------------------

QStringList RelayWdgt::getAvRelayStatuses()
{
    QStringList l;
    for(quint8 i = RELAY_STATE_UNKN; i <= RELAY_STATE_OFF_PRSBTTN; i++){
        l.append(RelayStateHelper::getRelayStatusHuman(i));
    }
    return l;
}

QStringList RelayWdgt::getSelectedNIs()
{
    return TableViewHelper::getSelectedRowsText(lastTv, 5);
}

QStringList RelayWdgt::getVisibleNIs()
{
    return TableViewHelper::getRowsText(lastTv, 5);

}

//-----------------------------------------------------------------------------

void RelayWdgt::clearPage()
{
    lastDateTimeMask = gHelper->getDateTimeMask();
    setTableHeaderWithData(getHeader());

}

//-----------------------------------------------------------------------------

void RelayWdgt::onUCEMeterSettingsChanged(UCEMeterSettings settings)
{
    //use EMeterListWdgt methods
    const MTableFullHouse outt = fromUCEMeterSettings(settings);
//    setTableData4amodelExt(outt.table, outt.ntable, getHeader4meterType(), getHeaderKeys4meterType().split(",") );
    setTableData4amodelExt4(outt, getHeader(), settings.validator);

    buttonsWidget->setEnabled(!outt.table.isEmpty());

    getLastRelayStateSmart(settings.validator);
}

//-----------------------------------------------------------------------------

void RelayWdgt::onUCEMeterRelayStateChanged(UCEMeterRelayState info)
{
    const QStringList nil = StandardItemModelHelper::getColText(5, -1, model);

    lastDateTimeMask = gHelper->getDateTimeMask();

    lRelayUpdate = EMeterListWdgt::fromUCEMeterRelayState(info, nil, lastDateTimeMask);

    emit updateRelayStatusTmr();
}

//-----------------------------------------------------------------------------

void RelayWdgt::getLastRelayStateSmart(const UCDataState &validator)
{
    if(validator.allDataIsReceived){
        const UCEMeterRelayState relaystate = gHelper->ucDeviceTreeW->getCachedUCEMeterRelayState();

        if(!relaystate.validator.dtlastupdate.isValid() || validator.dtlastupdate > relaystate.validator.dtlastupdate)
            getLastRelayState();
        else
            onUCEMeterRelayStateChanged(relaystate);
    }

}

//-----------------------------------------------------------------------------

void RelayWdgt::getLastRelayState()
{
    QString m;
    if(!gHelper->ucDeviceTreeW->getUCEMeterRelayState(accessibleName(), m))
        emit showMessage(m);
}

//-----------------------------------------------------------------------------

void RelayWdgt::updateRelayStatus()
{


    QElapsedTimer time;
    time.start();
    QStringList nichanged;

    const int imax = model->rowCount();
    int i = 0;


    for( ; i < imax; i++){

        const QString ni = model->item(i, 5)->text();
        if(lRelayUpdate.checkednis.contains(ni))
            continue;

        lRelayUpdate.checkednis.append(ni);

        const EMeterListWdgt::LRelayState relay = lRelayUpdate.relaytable.value(ni);

//        return tr("Time,Main Relay,Second Relay,Meter,S/N,NI,Memo,Coordinate").split(",");


        model->item(i,0)->setText( (relay.msec > 0) ?
                                       QDateTime::fromMSecsSinceEpoch(relay.msec).toLocalTime().toString(lastDateTimeMask) :
                                       "");
        model->item(i,1)->setText(relay.stts);
        model->item(i,2)->setText(relay.stts2);

        model->item(i, 1)->setData(relay.icon, Qt::DecorationRole);

        if(relay.hasSecondary)
            model->item(i, 2)->setData(relay.icon2, Qt::DecorationRole);


        model->item(i, 1)->setToolTip(EMeterListWdgt::getPrettyRelayState(relay).join("<br>"));

        if(relay.hasSecondary)
            model->item(i, 2)->setToolTip(EMeterListWdgt::getPrettyRelayState(relay).join("<br>"));

        if(relay.msec > 0 && model->item(i, 1)->data(Qt::UserRole + 11).isValid() && model->item(i, 1)->data(Qt::UserRole + 11).toLongLong() != relay.msec){
            nichanged.append(ni);
        }
        model->item(i, 1)->setData(relay.msec, Qt::UserRole + 11);//data changed watcher




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
    qDebug() << "onElMeterRelayChanged updateRelayStatus " << imax << lRelayUpdate.checkednis.size();
}

//-----------------------------------------------------------------------------

//void RelayWdgt::setPageSett(const MyListStringList &listRows, const QVariantMap &col2data, const QStringList &headerH, const QStringList &header, const bool &hasHeader)
//{
////    const QString currentNI

//    const QString currNi = headerH.contains("NI") ? TableViewHelper::getCellValueOfcurrentRow(ui->tvTable, headerH.indexOf("NI")) : "";

//    StandardItemModelHelper::append2model(listRows, col2data, headerH, header, hasHeader, model);

//    if(!currNi.isEmpty())
//        TableViewHelper::selectRowWithThisCell(ui->tvTable, currNi, headerH.indexOf("NI"));

//    ui->widget->setDisabled(listRows.isEmpty());

//    setHasDataFromRemoteDevice();

//    emit resizeTv2content(ui->tvTable);


//    ui->tbFilter->setEnabled(true);

//}

//-----------------------------------------------------------------------------

void RelayWdgt::meterRelayStatus(QString ni, QDateTime dtLocal, quint8 mainstts, quint8 secondarystts)
{
    if(ni.isEmpty())
        return;

    const int row = StandardItemModelHelper::getRowFromNI(5, ni, model);
    if(row < 0)
        return;


    UCEMeterRelayStateOneRelay onerelay;
    onerelay.msec = dtLocal.toMSecsSinceEpoch();
    onerelay.main = mainstts;
    onerelay.secondary = secondarystts;


    lRelayUpdate.checkednis.removeOne(ni);
    lRelayUpdate.relaytable.insert(ni, EMeterListWdgt::fromUCEMeterRelayStateOneRelay(onerelay, lastDateTimeMask));

    emit stopTmrRelyaStatusTmr();

    updateRelayStatus();
    TableViewHelper::selectRowWithThisCell(lastTv, ni, 5);

}

//void RelayWdgt::onButtonLock(bool disable)
//{
//    sendActLock(!ui->widget_2->isEnabled(), disable);
//}

void RelayWdgt::doRelayOperationSelected(const quint8 &operation)
{
    doRelayOperation(getSelectedNIs(), operation);
}

void RelayWdgt::onCommandEMeterRelayOperations(quint8 operation, QStringList nis, QString senderName)
{
    if(senderName.isEmpty())
        return;
    doRelayOperation(nis, operation);
}


void RelayWdgt::initPage()
{
    createModelsV2("RelayWdgt", true);
    connectMessageSignal();


    QTimer *tmrrelay = new QTimer(this);
    tmrrelay->setInterval(333);
    tmrrelay->setSingleShot(true);
    connect(this, SIGNAL(stopTmrRelyaStatusTmr()), tmrrelay, SLOT(stop()));
    connect(this, SIGNAL(updateRelayStatusTmr()), tmrrelay, SLOT(start()));

    connect(tmrrelay, SIGNAL(timeout()), this, SLOT(updateRelayStatus()));


    connect(this, &RelayWdgt::openContextMenu, this, &RelayWdgt::onTvTableCustomContextMenuRequested);
    connect(lastTv, &QTableView::customContextMenuRequested, this, &RelayWdgt::onTvTableCustomContextMenuRequested);

    clearPage();

    connect(gHelper->ucDeviceTreeW, &UCDeviceTreeWatcher::onUCEMeterRelayStateChanged, this, &RelayWdgt::onUCEMeterRelayStateChanged);
    connect(gHelper->ucDeviceTreeW, &UCDeviceTreeWatcher::onUCEMeterSettingsChanged, this, &RelayWdgt::onUCEMeterSettingsChanged);

    connect(gHelper->ucDeviceTreeW, &UCDeviceTreeWatcher::onCommandEMeterRelayOperations, this, &RelayWdgt::onCommandEMeterRelayOperations);

    QString m;
    if(gHelper->ucDeviceTreeW->getCachedUCEMeterSettings().validator.dtlastupdate.isValid())
         onUCEMeterSettingsChanged(gHelper->ucDeviceTreeW->getCachedUCEMeterSettings());
    else
        updatePageContent(m);


    connect(this, &RelayWdgt::lockButtons, buttonsWidget, &RelayButtonsWdgt::lockButtons);
    connect(buttonsWidget, &RelayButtonsWdgt::onDoRelayOperationSelected, this ,&RelayWdgt::doRelayOperationSelected);

    buttonsWidget->createSelectionChecker(lastTv);


    QTimer::singleShot(11, this, SLOT(createMapWidgetLaterReadOnly()));
    emit onPageCanReceiveData();

//    readDefCommandOnUpdate();
}


void RelayWdgt::onTvTableCustomContextMenuRequested(const QPoint &pos)
{
    QList<QAction*> la = getRelayActions();

    createCustomMenu4tv(pos,
                        (GuiHelper::ShowReset|GuiHelper::ShowAdd2Filter|GuiHelper::ShowExport|GuiHelper::ShowOnlyCopy),
                        getCopyPastTag(), la);


//    gHelper->createCustomMenu(pos, ui->tvTable,
//                              (GuiHelper::ShowReset|GuiHelper::ShowExport|GuiHelper::ShowOnlyCopy), CLBRD_SMPL_PRXTBL,
//                              ShowMessageHelperCore::matildaFileName(windowTitle()), getRelayActions());
}


void RelayWdgt::onPbReadAll_clicked()
{
    doRelayOperation(getVisibleNIs(), RELAY_READ);
}

void RelayWdgt::createTopWidget()
{
    buttonsWidget = new RelayButtonsWdgt(this);
    getTopLayout()->insertWidget(0, buttonsWidget);
    buttonsWidget->setEnabled(false);
}


void RelayWdgt::doRelayOperation(const QStringList &listni, const quint8 &operation)
{
    if(gHelper->managerEnDisBttn.pbWriteDis){
        gHelper->showMessageSlot(tr("Dismissed. Another task is processing"));
        return;
    }

    if(listni.isEmpty()){
        gHelper->showMessageSlot(tr("no meters"));
        return;
    }

    QString mess;
    const QVariantMap map = QuickPollHelper::createPollMap4relay(listni,operation, mess);

    if(map.isEmpty())
        return;

    lastTv->clearSelection();
    StandardItemModelHelper::clearCells(listni, 5,QList<int>() << 0 << 1 << 2, model);

    emit setLastPageId(accessibleName());

    emit command4devStr(POLL_CODE_RELAY_OPERATIONS, QString("-ns %1 -operation %2")
                        .arg(map.value("-ns").toStringList().join(" ")).arg(map.value("-operation").toString()));
//    emit command4dev(POLL_CODE_RELAY_OPERATIONS, map);

}

QList<QAction *> RelayWdgt::getRelayActions()
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

