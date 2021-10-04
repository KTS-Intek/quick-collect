#include "zbyrmeterlistmedium.h"


#include <QDebug>
#include <QTimer>


///[!] guisett-shared-core
#include "src/nongui/tableheaders.h"
#include "src/nongui/settloader.h"


///[!] device-poll
#include "src/meter/zbyratorfilesetthelper.cpp"


///[!] quick-collect
#include "zbyrator-src/src/zbyratordatacalculation.h"
#include "zbyrator-src/src/zbyratordatabasemedium.h"


///[!] widgets-shared
#include "gui-src/standarditemmodelhelper.h"
#include "zbyrator-src/wdgt/compliterlistdialog.h"


///[!] matilda-bbb-settings
#include "src/matilda/serialporthelper.h"


///[!] type-converter
#include "src/shared/networkconverthelper.h"


///[!] guisett-shared
#include "zbyrator-src/src/startexchangehelper.h"




///[!] zbyrator-shared
#include "src/zbyrator-v2/watermeterhelper.h"
#include "src/meter/relaystatehelper.h"
#include "src/zbyrator-v2/quickpollhelper.h"


///[!] zbyrator-settings
#include "src/zbyrator-v2/metersloader.h"


///[!] zbyrator-base
#include "src/ipc/zbyratorsocket.h"

///[!] zbyrator-meters
#include "src/meter/meterpluginloader.h"



///[!] quick-collect-gui-core
#include "quick-collect-gui-core/emulator/peredavatorcover.h"


///[!] guisett-shared-core
#include "src/nongui/meterstatehelper4gui.h"


///[!] matilda-bbb-clientside
#include "src/matilda-conf/classmanagerv1.cpp"
#include "src/matilda-conf/classmanagerv5.h"
#include "src/matilda-conf/classmanagerv11.h"


#include "myucdevicetypes.h"
#include "moji_defy.h"

//---------------------------------------------------------------------

ZbyrMeterListMedium::ZbyrMeterListMedium(QObject *parent) : GuiIfaceMedium(parent)
{
    QTimer *t = new QTimer(this);
    t->setSingleShot(true);
    t->setSingleShot(111);
    connect(this, SIGNAL(startTmrSaveLater()), t, SLOT(start())) ;
    connect(t, SIGNAL(timeout()), this, SLOT(onSaveLater()) );

//    ifaceLoaderPrimary - is already created
    createUcDevTree();
    createDataCalculator();
    createDatabaseMedium();
    createLocalSocketObject();
    metersStatusManager = new LastMetersStatusesManager(this);
    connect(this, &ZbyrMeterListMedium::add2fileMeterRelayStatus, metersStatusManager, &LastMetersStatusesManager::add2fileMeterRelayStatus);
//    connect(metersStatusManager, &LastMetersStatusesManager::onFileSaved, [=]{
//        emit startTmrUpdateRelayStatuses(1111);
//    });

    pageModeUpdated = false;


    QTimer::singleShot(3333, this, SLOT(createPeredavatorEmbeeManagerLater()));

}

UCDataState ZbyrMeterListMedium::getTemplateValidator()
{
    UCDataState validator;
    validator.allDataIsReceived = true;
    validator.dtlastupdate = QDateTime::currentDateTimeUtc();
    return validator;
}

void ZbyrMeterListMedium::importGroups2metersFile()
{
    QString err;
    const qint64 count = MetersLoader::importElectricityMetersGroupsAsMeters(err);
    MetersLoader::removeGroupsFile();
    if(count > 0){
        emit showMessage(tr("Electricity meter groups were imported as electricity power centers"));
    }

}

void ZbyrMeterListMedium::resetVariables4pollStarted()
{
    if(pageModeUpdated){
        pageModeUpdated = false;
        return;
    }
    lastPageMode = -1;//reset

}

void ZbyrMeterListMedium::activateEmul2DeviceType()
{


    auto systeminfo = ucDeviceTreeW->getCachedUCSystemInfo();
    systeminfo.devType = DEV_POLL_EMULATOR_L2;
    systeminfo.SN = "EMUL";
    systeminfo.validator.dtlastupdate = QDateTime::currentDateTimeUtc();
    systeminfo.validator.allDataIsReceived = true;
    ucDeviceTreeW->setUCSystemInfo(systeminfo);
}



//---------------------------------------------------------------------

void ZbyrMeterListMedium::onAllMetersSlot(UniversalMeterSettList allMeters)
{
    if(allMeters.isEmpty())
        return;

    lastAllMeters = allMeters;

    UCEMeterSettings emeter;
    UCWMeterSettings wmeter;

    UCPMeterSettings pmeter;

    QMap<quint8, UniversalMeterSettList> map2meters;
    for(int i = 0, imax = allMeters.size(); i < imax; i++){
        const UniversalMeterSett m = allMeters.at(i);

        switch(m.deviceType){
        case UC_METER_ELECTRICITY   : emeter.eMeterContainer.append(universalMeterSett2emeterSettings(m))  ; break;
        case UC_METER_WATER         : wmeter.wMeterContainer.append(universalMeterSett2wmeterSettings(m)); break;
        case UC_METER_PULSE         : pmeter.pMeterContainer.append(universalMeterSett2pmeterSettings(m)); break;

        }

        if(true){
            UniversalMeterSettList l = map2meters.value(m.deviceType);
            l.append(m);
            map2meters.insert(m.deviceType, l);
        }

    }



    emeter.validator = wmeter.validator = pmeter.validator = getTemplateValidator();

    ucDeviceTreeW->setUCEMeterSettings(emeter);//first
    ucDeviceTreeW->setUCWMeterSettings(wmeter);//second
    ucDeviceTreeW->setUCPMeterSettings(pmeter);//second





    onElectricitylistOfMeters(allMeters, MyNi2model(), true);
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::meterElectricityModelChanged(QVariantList meters)
{
    meterModelChanged(meters, UC_METER_ELECTRICITY);


}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::meterWaterModelChanged(QVariantList meters)
{
    meterModelChanged(meters, UC_METER_WATER);


}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::meterPulseModelChanged(QVariantList meters)
{
    meterModelChanged(meters, UC_METER_PULSE);


}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::meterModelChanged(const QVariantList &meters, const quint8 &meterType)
{
    emit onConfigChanged(MTD_EXT_CUSTOM_COMMAND_2, true);
    lastSaveMeterList.lastMeterList = meters;
    lastSaveMeterList.deviceType = meterType;
    emit startTmrSaveLater();
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::onSaveLater()
{
    const quint8 deviceType = lastSaveMeterList.deviceType;
    const QString mess = MetersLoader::saveMetersByType(deviceType, lastSaveMeterList.lastMeterList);
    if(!mess.isEmpty())
        qDebug() << "ZbyrMeterListMedium mess " << mess;
    emit onConfigChanged(MTD_EXT_COMMAND_RELOAD_SETT, true);



    //MTD_EXT_COMMAND_RELOAD_SETT
    lastSaveMeterList.lastMeterList.clear();
    lastSaveMeterList.deviceType = UC_METER_UNKNOWN;

//    doReloadListOfElectricityMeters();
//    doReloadListOfMeters(UC_METER_UNKNOWN);
    emit onReloadAllMeters2zbyrator();

    onGetUCEMeterRelayState("ZbyrMeterListMedium::onSaveLater");
}


//---------------------------------------------------------------------

void ZbyrMeterListMedium::doReloadListOfMeters(quint8 deviceType)
{
    emit sendMeAlistOfMeters(deviceType);
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::onAlistOfMeters(quint8 deviceType, UniversalMeterSettList activeMeters, MyNi2model switchedOffMeters)
{
//    emit onAddMeters(deviceType, activeMeters, switchedOffMeters, false);
    switch (deviceType) {

    case UC_METER_ELECTRICITY: onElectricitylistOfMeters(activeMeters, switchedOffMeters, false); break;

    default:
        break;
    }


}



//---------------------------------------------------------------------

void ZbyrMeterListMedium::createDataCalculator()
{
    ZbyratorDataCalculation *c = new ZbyratorDataCalculation;
    QThread *t = new QThread;
    c->moveToThread(t);

//    connect(this, SIGNAL(destroyed(QObject*)), t, SLOT(quit()) );

    connect(this, &ZbyrMeterListMedium::killAllObjects, c, &ZbyratorDataCalculation::kickOffObject);
    connect(c, SIGNAL(destroyed(QObject*)), t, SLOT(quit()));
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));

    connect(t, SIGNAL(started()), c, SLOT(onThreadStarted()) );


//    connect(this, &ZbyrMeterListMedium::onAddMeters     , c, &ZbyratorDataCalculation::onAlistOfMeters  );
    connect(this, &ZbyrMeterListMedium::appendMeterData , c, &ZbyratorDataCalculation::appendMeterData  );
    connect(this, &ZbyrMeterListMedium::onPollStarted   , c, &ZbyratorDataCalculation::onPollStarted    );
    connect(this, &ZbyrMeterListMedium::onUconStartPoll , c, &ZbyratorDataCalculation::onUconStartPoll  );

    connect(this, &ZbyrMeterListMedium::onMeterPollCancelled, c, &ZbyratorDataCalculation::onMeterPollCancelled);

//    connect(c, &ZbyratorDataCalculation::appendData2model, this, &ZbyrMeterListMedium::appendData2model);
//    connect(c, &ZbyratorDataCalculation::setLblWaitTxt   , this, &ZbyrMeterListMedium::setLblWaitTxt);
//    connect(c, &ZbyratorDataCalculation::updateHashSn2meter, this, &ZbyrMeterListMedium::updateHashSn2meter);


    connect(c, &ZbyratorDataCalculation::onUCLastReceivedDeviceRecordsQuickCollectChanged, this, &ZbyrMeterListMedium::onUCLastReceivedDeviceRecordsQuickCollectChanged);
    // ucDeviceTreeW, &UCDeviceTreeWatcher::setUCLastReceivedDeviceRecords);

    connect(ucDeviceTreeW, &UCDeviceTreeWatcher::onUCEMeterSettingsChanged, c, &ZbyratorDataCalculation::onUCEMeterSettingsChanged);
    connect(ucDeviceTreeW, &UCDeviceTreeWatcher::onUCWMeterSettingsChanged, c, &ZbyratorDataCalculation::onUCWMeterSettingsChanged);

//    connect(this, &ZbyrMeterListMedium::setVirtualMetersSett, c, &ZbyratorDataCalculation::setVirtualMetersSett);

    t->start();

}

void ZbyrMeterListMedium::createDatabaseMedium()
{
    //data from data to GUI, UCEmulator
    ZbyratorDatabaseMedium *m = new ZbyratorDatabaseMedium(ucDeviceTreeW);



    QThread *t = new QThread;
    m->moveToThread(t);
    t->setObjectName("ZbyratorDatabaseMedium");



    connect(this, &ZbyrMeterListMedium::killAllObjects, m, &ZbyratorDatabaseMedium::kickOffObject);


    connect(m, SIGNAL(destroyed(QObject*)), t, SLOT(quit()) );
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));

    connect(t, SIGNAL(started()), m, SLOT(onThreadStarted()) );
//    connect(this, &ZbyrMeterListMedium::onAddMeters     , m, &ZbyratorDatabaseMedium::onAlistOfMeters);


//    connect(this, SIGNAL(data2dbMedium(quint16,QVariant)), m, SLOT(data2matilda4inCMD(quint16,QVariant)));//do not use it
    connect(this, SIGNAL(stopReadDatabase()), m, SLOT(stopOperationSlot()) );


    connect(m, SIGNAL(setPbReadEnableDisable(bool)), this, SIGNAL(setPbReadEnableDisable(bool)));

//    connect(m, SIGNAL(appendDataDatabase(QVariantHash)), this, SIGNAL(appendDataDatabase(QVariantHash)));
//    connect(m, SIGNAL(appendDataDatabaseMJ(QVariantHash)), this, SIGNAL(appendDataDatabaseMJ(QVariantHash)));

    connect(m, SIGNAL(setLblWaitTxtDatabase(QString)), this, SIGNAL(setLblWaitTxtDatabase(QString)));

//    connect(this, &ZbyrMeterListMedium::setVirtualMetersSett, m, &ZbyratorDatabaseMedium::setVirtualMetersSett); that is not checked


    QTimer::singleShot(999, t, SLOT(start()));
//    t->start();


}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::onAllStatHash(QStringList allstat)
{
    UCExchangeOfTheMetersInfo statistictable;
    statistictable.validator = getTemplateValidator();
    bool isBroken;
    statistictable.stattable = ClassManagerV1::fromList2UCStatTable(allstat, isBroken);

    if(isBroken)
        return;

    ucDeviceTreeW->setUCExchangeOfTheMetersInfo(statistictable);
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::onTaskCanceled(quint8 pollCode, QString ni, qint64 dtFinished, quint8 rez)
{

    if(rez == 0) // RD_EXCHANGE_DONE)
        return;
    Q_UNUSED(pollCode);
    const QString stts = StartExchangeHelper::getStts4rez(rez);

    emit appendAppLog(tr("%3: IN: %1, one task removed, result is %2").arg(ni).arg(stts).arg(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss")));

    const QDateTime dtLocal = QDateTime::fromMSecsSinceEpoch(dtFinished).toLocalTime();

    switch(lastPageMode){
    case 0: emit onMeterPollCancelled(ni, stts, dtFinished); break;//poll page
    case 1: waterMeterSchedulerStts(ni, dtLocal, stts, QVariantHash(), ""); break;
    case 2: meterRelayStatus(ni, dtLocal,  RELAY_STATE_UNKN, RELAY_STATE_UNKN); break; //relay
    case 5: emit meterDateTimeDstStatus(ni, dtLocal, stts); break;
    }

    emit onTaskTableChanged();
}
//---------------------------------------------------------------------
void ZbyrMeterListMedium::setLastPageId(QString accsblName)
{
    //    return QString("Poll;Relay;Queue;Statistic of the exchange;Date and time;Meter address;Check Connection Tool;Other;Interface").split(";");
    lastPageMode = StartExchangeHelper::getChList().indexOf((accsblName));
    pageModeUpdated = true;
}


//---------------------------------------------------------------------
void ZbyrMeterListMedium::onReloadAllZbyratorSettingsLocalSocket()
{
    mapMeters2pages.insert("Scheduler for water meters", LastList2pages());
    emit onReloadAllMeters2zbyrator();
    emit reloadSavedSleepProfiles();

}
//---------------------------------------------------------------------
void ZbyrMeterListMedium::createPeredavatorEmbeeManagerLater()
{
    createPeredavatorEmbeeManager();
}
//---------------------------------------------------------------------
void ZbyrMeterListMedium::command4devSlotLocalSocket(quint16 command, QString args)
{
    qDebug() << "a command from the local socket " << command << args;
    switch(command){
    case 0: emit pbStopAnimateClick(); emit stopApiAddressator(); return;//stop all
    case ZBRTR_OPEN_DIRECT_ACCESS: startApiAddressatorSlot(quickcollectstate.lastTcpMode, quickcollectstate.lastTcpPort); return; //open da
    }
    emit onExternalCommandProcessed();

    command4devSlot(command, args);

}

//---------------------------------------------------------------------
void ZbyrMeterListMedium::mWrite2RemoteDev(quint16 command, QVariant dataVar)
{
    switch(command){
    case COMMAND_WRITE_ELMTRRELAY_OPERATION:{
        if(pbWriteDis)
            return;

        const QStringList listni = dataVar.toHash().value("nis").toStringList();
        if(listni.isEmpty()){
            emit showMessage(tr("no meters"));
            return;
        }
        const int operation = dataVar.toHash().value("mode").toInt();
        QString mess;
        const QVariantMap map = QuickPollHelper::createPollMap4relay(listni, operation, mess);

        if(map.isEmpty())
            return;

        setLastPageId("Relay");

        command4devSlot(POLL_CODE_RELAY_OPERATIONS, map);

        break;}//realy operations, main relay
    case COMMAND_READ_ELMTRRELAY_TABLE:{

        //        emit startTmrUpdateRelayStatuses(111);

        break;} //send last realy statuses
    }

    qDebug() << "onElMeterRelayChanged mWrite2RemoteDev "  << command;

}

//---------------------------------------------------------------------
void ZbyrMeterListMedium::updateRelayStatuses4meterlist()
{

    QString errmessage;
    const QMap<QString,UCEMeterRelayStateOneRelay> relaysttsmap = LastMetersStatusesManager::getLastRelayStatusesMap(errmessage);//it saves the file automatically

    qDebug() << "onElMeterRelayChanged updateRelayStatuses4meterlist " << relaysttsmap.size() << errmessage;

    updateRelayStatuses4meterlistExt(relaysttsmap);
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::setPbWriteDis(bool disabled)
{
    pbWriteDis = disabled;
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::command2extension(quint16 extName, quint16 command, QVariant data)
{
    qDebug() << "Extname " << extName << command;
    emit onConfigChanged(command, data);
    onReloadAllZbyratorSettingsLocalSocket();
}


//---------------------------------------------------------------------

void ZbyrMeterListMedium::onGetUCEMeterSettings(QString senderName)
{
//    Q_UNUSED(senderName);
    onGetUCSupportedMetersInfo(senderName);
    QTimer::singleShot(111, this, SIGNAL(onReloadAllMeters2zbyrator()));
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::onPutUCEMeterSettings(UCEMeterSettings settings, QString senderName)
{
    QStringList errl;
    const QVariantList varl = ClassManagerV1::fromUCEMeterSettings(settings, errl);

    if(!errl.isEmpty()){
        qDebug() << "onPutUCEMeterSettings " << senderName << errl;
        emit showMessage(errl.join("<br>"));
        return;
    }

    meterElectricityModelChanged(varl);//the old method looks ok

}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::onPutUCWMeterSettings(UCWMeterSettings settings, QString senderName)
{
    QStringList errl;
    const QVariantList varl = ClassManagerV5::fromUCWMeterSettings(settings, errl);

    if(!errl.isEmpty()){
        qDebug() << "fromUCWMeterSettings " << senderName << errl;
        emit showMessage(errl.join("<br>"));
        return;
    }

    meterWaterModelChanged(varl);//the old method looks ok
}

void ZbyrMeterListMedium::onPutUCPMeterSettings(UCPMeterSettings settings, QString senderName)
{
    qDebug() << "ZbyrMeterListMedium::onPutUCPMeterSettings save the pulse meters " << senderName ;

    QStringList errl;
    const QVariantList varl = ClassManagerV11::fromUCPMeterSettings(settings, errl);

    if(!errl.isEmpty()){
        qDebug() << "fromUCPMeterSettings " << senderName << errl;
        emit showMessage(errl.join("<br>"));
        return;
    }

    meterPulseModelChanged(varl);//the old method looks ok

}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::onGetUCEMeterRelayState(QString senderName)
{
    Q_UNUSED(senderName);
    QTimer::singleShot(111, this, SLOT(updateRelayStatuses4meterlist()));
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::meterRelayStatus(QString ni, QDateTime dtLocal, quint8 mainstts, quint8 secondarystts)
{
    emit add2fileMeterRelayStatus(ni, dtLocal, mainstts, secondarystts);
    //this method is only for cancelled tasks
//    if(dtLocal.isValid())
//        return;//wait until the file is saved,
    UCEMeterRelayState erelay = ucDeviceTreeW->getCachedUCEMeterRelayState();
    UCEMeterRelayStateOneRelay onerelay = erelay.ni2relay.value(ni);

    onerelay.main = mainstts;
    onerelay.secondary = secondarystts;
    onerelay.msec = dtLocal.toMSecsSinceEpoch();

    erelay.ni2relay.insert(ni, onerelay);
    erelay.validator = getTemplateValidator();
    ucDeviceTreeW->setUCEMeterRelayState(erelay);

}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::onGetUCSupportedMetersInfo(QString senderName)
{

    Q_UNUSED(senderName);
    UCSupportedMetersInfo emeter = ucDeviceTreeW->getCachedUCSupportedMetersInfoElectricity();
    UCSupportedMetersInfo wmeter = ucDeviceTreeW->getCachedUCSupportedMetersInfoWater();
    UCSupportedMetersInfo pmeter = ucDeviceTreeW->getCachedUCSupportedMetersInfoPulse();

    if(emeter.validator.dtlastupdate.isValid() && wmeter.validator.dtlastupdate.isValid())
        return;

    emeter = wmeter = pmeter = UCSupportedMetersInfo();//reset all


    const MeterPluginInfo info = MeterPluginLoader::loadAboutPlugin(PathsResolver::path2pluginsDir());// hash;

    for(int i = 0, imax = info.allModels.size(); i < imax; i++){

        const QString plgname = info.allModels.at(i);
        const MeterPluginOneMeterInfo oneplg = info.model2info.value(plgname);


        UCMPlgParams plgparam;
        plgparam.supportedMeters = oneplg.suppMeters.split(",");
        if(plgparam.supportedMeters.isEmpty() || plgparam.supportedMeters.first().isEmpty()){
            plgparam.supportedMeters.clear();
            plgparam.supportedMeters.append(plgname);
        }

        const QString regStr = oneplg.regExpRules;

        if(regStr.split("$^").length() == 2 && regStr.left(1) == "^" && regStr.right(1) == "$"){
            plgparam.niRule = regStr.split("$^").first() + "$";
            plgparam.passwordRule = "^" + regStr.split("$^").last();
        }

        if(plgparam.niRule.isEmpty())
            plgparam.niRule = regStr.isEmpty() ? "^(.){32}$" : regStr;

        if(plgparam.passwordRule.isEmpty())
            plgparam.passwordRule = regStr.isEmpty() ? "^(.){32}$" : regStr;


        switch(oneplg.deviceType){
        case UC_METER_ELECTRICITY   : emeter.pluginNames.append(plgname); emeter.pluginParams.insert(plgname, plgparam); break;
        case UC_METER_WATER         : wmeter.pluginNames.append(plgname); wmeter.pluginParams.insert(plgname, plgparam); break;
        case UC_METER_PULSE         : pmeter.pluginNames.append(plgname); pmeter.pluginParams.insert(plgname, plgparam); break;

        }
    }


    emeter.validator = wmeter.validator = pmeter.validator = getTemplateValidator();

    ucDeviceTreeW->setUCSupportedMetersInfoEMeter(emeter);
    ucDeviceTreeW->setUCSupportedMetersInfoWMeter(wmeter);
    ucDeviceTreeW->setUCSupportedMetersInfoPMeter(pmeter);


}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::onGetUCWMeterSettings(QString senderName)
{
    onGetUCEMeterSettings(senderName);//it reloads all meters;
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::onGetUCPMeterSettings(QString senderName)
{
    onGetUCEMeterSettings(senderName);//it reloads all meters;

}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::onCommand2killMeterTasks(QList<quint32> lDecimalTaskIDs, QString senderName)
{
    Q_UNUSED(senderName);

    QStringList lIdsHex;

    for(int i = 0, imax = lDecimalTaskIDs.size(); i < imax; i++){
        lIdsHex.append(QString::number(lDecimalTaskIDs.at(i), 16));
    }

    if(lIdsHex.isEmpty()){
        qDebug() << "ZbyrMeterListMedium::onCommand2killMeterTasks " << senderName;
        return;
    }


    emit onZbyratorConfigChanged(MTD_EXT_CUSTOM_COMMAND_13, lIdsHex.join("|"));

}

void ZbyrMeterListMedium::vanishFinishedTasks()
{
    emit onZbyratorConfigChanged(MTD_EXT_CUSTOM_COMMAND_14, true);

}

void ZbyrMeterListMedium::onUCWMeterSettingsChanged(UCWMeterSettings settings)
{
    waterNi2sn.clear();
    for(int i = 0, imax = settings.wMeterContainer.size(); i < imax; i++){
        const UCPollDeviceSettings onerow = settings.wMeterContainer.at(i).baseSettings;
        waterNi2sn.insert(onerow.ni, onerow.sn);
    }
}

void ZbyrMeterListMedium::waterMeterSchedulerStts(QString ni, QDateTime dtLocal, QString stts, QVariantHash sheduler, QString src)
{

    Q_UNUSED(stts);
    if(ni.isEmpty())
        return;


    UCWMeterLastProfileOneRow onerow;
    onerow.src = src.toUInt();
    onerow.sleepProfileLine = WaterMeterHelper::oneProfile2lineSmpl(sheduler);
    onerow.dtmsec = dtLocal.toMSecsSinceEpoch();
    onerow.sn = waterNi2sn.value(ni);

    UCWMeterLastProfilesSettings settings = ucDeviceTreeW->getCachedUCWMeterLastProfilesSettings();
    settings.lastWaterMeterProfiles.insert(ni, onerow);
    settings.validator.allDataIsReceived = true;
    settings.validator.dtlastupdate = dtLocal;

    ucDeviceTreeW->setUCWMeterLastProfilesSettings(settings);
}

void ZbyrMeterListMedium::onCommandOpenDirectAccess(UCOpenDirectAccessCommand request, QString senderName)
{
    Q_UNUSED(senderName);

    resetStopDirectAccess();

    emit setUCDirectAccessServiceParams(request.ipmode, request.defport); //it tells to stop daservice
//    ucDeviceTreeW->setDirectAccessChannelState(UCDA_CHANNEL_STATE_CLOSED);

    if(request.openmode != COMMAND_DA_CLOSE){
        sendIfaceSettings();
        ucDeviceTreeW->setDirectAccessChannelState(UCDA_CHANNEL_STATE_OPENNING);
        //    sendThisCommand(COMMAND_WRITE_DA_OPEN_CLOSE, request.openmode, senderName);

        emit startStopDirectAccessService(true);
        ucDeviceTreeW->setDirectAccessChannelState(UCDA_CHANNEL_STATE_OPEN);

    }

//    if(request.openmode == COMMAND_DA_CLOSE){
//        emit stopApiAddressator();
//    }else{
//        emit startApiAddressatorSlot(request.ipmode, request.defport);
//    }
}

void ZbyrMeterListMedium::resetStopDirectAccess()
{
    //please stop DA service correctly, it is called when onConnectionStateChanged
    const quint8 state = ucDeviceTreeW->getDirectAccessChannelState();
    if(state == UCDA_CHANNEL_STATE_OPEN || state == UCDA_CHANNEL_STATE_OPENNING){
        ucDeviceTreeW->setDirectAccessChannelState(UCDA_CHANNEL_STATE_CLOSING);
    }
    emit startStopDirectAccessService(false);
    ucDeviceTreeW->setDirectAccessChannelState(UCDA_CHANNEL_STATE_CLOSED);
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::setVirtualMetersSett(NI2vmGSNsett vmsett)
{
    const QList<QString> lk = vmsett.keys();
    UCEVirtualMetersSettings settings;

    settings.validator = getTemplateValidator();

    for(int i = 0, imax = lk.size(); i < imax; i++){
        const QString ni = lk.at(i);
        const ZVirtualMeters onemeter = vmsett.value(ni);


        QList<QString> gsnl = onemeter.keys();
        std::sort(gsnl.begin(), gsnl.end());


        QVector<UCEOneVirtualMeter> onevml;

        for(int j = 0, jmax = onevml.size(); j < jmax; j++){
            const QString gsn = gsnl.at(j);
            UCEOneVirtualMeter onem;
            onem.gsn = gsn;
            onem.memo = onemeter.value(gsn).memo;
            onevml.append(onem);
        }
        settings.ni2vmeter.insert(ni, onevml);

//        QString gsn;//<AAAA><BBBB><CCCC>
//        QString memo;

//        bool isValid;ZVirtualMeters
//        bool isSinglePhase;
//        //adresses
//        qint32 aPhaseAddr;
//        qint32 bPhaseAddr;
//        qint32 cPhaseAddr;


//        QString memo;
    }

    ucDeviceTreeW->setUCEVirtualMetersSettings(settings);
}

void ZbyrMeterListMedium::sendAllMeters()
{
    emit onAllMeters(lastAllMeters);
}


//---------------------------------------------------------------------

void ZbyrMeterListMedium::createUcDevTree()
{
    ucDeviceTreeW = new UCDeviceTreeWatcher(true, true, this);
        ucDeviceTreeW->setConnectionState(UCCONNECT_STATE_CONNECTIONREADY);
        ucDeviceTreeW->setAccessLevel(MTD_USER_ADMIN);
        ucDeviceTreeW->setProtocolVersion(MATILDA_PROTOCOL_VERSION);
    //    ucDeviceTreeW->setDirectAccessChannelState(UCDA_CHANNEL_STATE_CLOSED);
    //9.31
    connect(ucDeviceTreeW, &UCDeviceTreeWatcher::onGetUCEMeterSettings, this, &ZbyrMeterListMedium::onGetUCEMeterSettings);
    connect(ucDeviceTreeW, &UCDeviceTreeWatcher::onGetUCEMeterRelayState, this, &ZbyrMeterListMedium::onGetUCEMeterRelayState);
    connect(ucDeviceTreeW, &UCDeviceTreeWatcher::onPutUCEMeterSettings, this, &ZbyrMeterListMedium::onPutUCEMeterSettings);

    //9.61
    connect(ucDeviceTreeW, &UCDeviceTreeWatcher::onGetUCWMeterSettings, this, &ZbyrMeterListMedium::onGetUCWMeterSettings);
    connect(ucDeviceTreeW, &UCDeviceTreeWatcher::onPutUCWMeterSettings, this, &ZbyrMeterListMedium::onPutUCWMeterSettings);

    //6.60 View tasks of the meter
    connect(ucDeviceTreeW, &UCDeviceTreeWatcher::onCommand2killMeterTasks, this, &ZbyrMeterListMedium::onCommand2killMeterTasks);


    //5.5
    connect(ucDeviceTreeW, &UCDeviceTreeWatcher::onCommandOpenDirectAccess, this, &ZbyrMeterListMedium::onCommandOpenDirectAccess);
    connect(ucDeviceTreeW, &UCDeviceTreeWatcher::onSetDaForwardintSettings, this, &ZbyrMeterListMedium::onSetDaForwardintSettings);







    connect(ucDeviceTreeW, &UCDeviceTreeWatcher::onUCWMeterSettingsChanged, this, &ZbyrMeterListMedium::onUCWMeterSettingsChanged);


    //
    connect(ucDeviceTreeW, &UCDeviceTreeWatcher::onGetUCPMeterSettings, this, &ZbyrMeterListMedium::onGetUCPMeterSettings);
    connect(ucDeviceTreeW, &UCDeviceTreeWatcher::onPutUCPMeterSettings, this, &ZbyrMeterListMedium::onPutUCPMeterSettings);

    QTimer::singleShot(3333, this, SIGNAL(reloadSavedSleepProfiles()));//zbyrator starts in 1111 msec, so it must be after

//    connect(ucDeviceTreeW, &UCDeviceTreeWatcher::ongetucs)

//ucDeviceTreeW->setUCSupportedMetersInfoEMeter();




}
//---------------------------------------------------------------------
UCEMeterSettingsOneRow ZbyrMeterListMedium::universalMeterSett2emeterSettings(const UniversalMeterSett &m)
{
    UCEMeterSettingsOneRow onerow;
    onerow.baseSettings = universalMeterSett2baseSettings(m);
    onerow.meterSettings = universalMeterSett2meterSettings(m);

    onerow.tariff = m.tariff;
    onerow.enrg = m.enrg;


    onerow.transformerSettings.hasTransformer = m.transformer.hasTransformer;
    onerow.transformerSettings.only4display = m.transformer.only4display;
    onerow.transformerSettings.ucDividend = m.transformer.ucDividend;
    onerow.transformerSettings.ucDivisor = m.transformer.ucDivisor;
    onerow.transformerSettings.icDividend = m.transformer.icDividend;
    onerow.transformerSettings.icDivisor = m.transformer.icDivisor;
    onerow.transformerSettings.endUcoefitient = m.transformer.endUcoefitient;
    onerow.transformerSettings.endIcoefitient = m.transformer.endIcoefitient;
    onerow.transformerSettings.endCoefitient = m.transformer.endCoefitient;

    return onerow;
}
//---------------------------------------------------------------------
UCWMeterSettingsOneRow ZbyrMeterListMedium::universalMeterSett2wmeterSettings(const UniversalMeterSett &m)
{
    UCWMeterSettingsOneRow onerow;
    onerow.baseSettings = universalMeterSett2baseSettings(m);
    onerow.meterSettings = universalMeterSett2meterSettings(m);
    onerow.sleepProfileLine = m.enrg;
    return onerow;

}

//---------------------------------------------------------------------

UCPMeterSettingsOneRow ZbyrMeterListMedium::universalMeterSett2pmeterSettings(const UniversalMeterSett &m)
{
    UCPMeterSettingsOneRow onerow;
    onerow.baseSettings = universalMeterSett2baseSettings(m);
    onerow.meterSettings = universalMeterSett2meterSettings(m);
    onerow.channelsSettings = ClassManagerV11::mapMeterChannelsFromLine(m.enrg);

//    onerow.channelsSettings  sleepProfileLine = m.enrg;
    return onerow;
}

//---------------------------------------------------------------------

UCPollDeviceSettings ZbyrMeterListMedium::universalMeterSett2baseSettings(const UniversalMeterSett &m)
{
//    QPointF coordinate;
//    QString memo;


    UCPollDeviceSettings baseSettings;
    baseSettings.ni = m.ni;
    baseSettings.sn = m.sn;
    baseSettings.model = m.model;
    baseSettings.vrsn = m.version;
    baseSettings.isPollEn = m.pollEnbl;
    bool ok;
    baseSettings.coordinate = ConvertAtype::coordinateFromStr(m.coordinate, ok);
    baseSettings.memo = m.memo;

    return baseSettings;
}
//---------------------------------------------------------------------
UCMeterDeviceSettings ZbyrMeterListMedium::universalMeterSett2meterSettings(const UniversalMeterSett &m)
{
    UCMeterDeviceSettings meterSettings;

    meterSettings.dta = m.disableTimeCorrection;
    meterSettings.input = m.powerin;
    meterSettings.password = m.passwd;

    return meterSettings;
}

//---------------------------------------------------------------------
bool ZbyrMeterListMedium::metersChanged(QMap<QString, ZbyrMeterListMedium::LastList2pages> &mapMeters2pages, const QString &key, const LastList2pages &lastMeters2pagesL)
{
    if((mapMeters2pages.contains(key) && metersChanged(mapMeters2pages.value(key), lastMeters2pagesL)) ){ //|| !mapMeters2pages.contains(key)){
        mapMeters2pages.insert(key, lastMeters2pagesL);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------
bool ZbyrMeterListMedium::metersChanged(const ZbyrMeterListMedium::LastList2pages &lastMeters2pages, const ZbyrMeterListMedium::LastList2pages &lastMeters2pagesL)
{
     return (lastMeters2pages.listNI != lastMeters2pagesL.listNI || lastMeters2pages.mainParams != lastMeters2pagesL.mainParams);
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::onElectricitylistOfMeters(const UniversalMeterSettList &activeMeters, const MyNi2model &switchedOffMeters, const bool &checkOffMeters)
{
    const int activeMetersSize = activeMeters.size();
    const int switchedOffMetersSize = switchedOffMeters.size();


    int pollOnMeters = 0;
    int pollOffMeters = switchedOffMetersSize;

    int meterElectricityActive = 0;
    int meterWaterActive = 0;

    int meterGasActive = 0;
    int meterPulseActive = 0;
//    QStringList listNiNotchanged;



    for(int i = 0; i < activeMetersSize; i++){

        const UniversalMeterSett m = activeMeters.at(i);


        if(checkOffMeters && !m.pollEnbl){
            pollOffMeters++;
            continue;
        }
        pollOnMeters++;



        switch(m.deviceType){
        case UC_METER_ELECTRICITY:{
            meterElectricityActive++;

            break;}

        case UC_METER_WATER:{
            meterWaterActive++;
            break;}

        case UC_METER_GAS:{
            meterGasActive++;
            break;}

        case UC_METER_PULSE:{
            meterPulseActive++;
            break;}
        }

    }




    emit onUpdatedSavedList(pollOnMeters, pollOffMeters, meterElectricityActive, meterWaterActive, meterGasActive, meterPulseActive);



}




//---------------------------------------------------------------------

MyListStringList ZbyrMeterListMedium::getRowsList(QMap<QString, QStringList> &mapPage, const QStringList &listNiNotchanged, const QMap<QString, QStringList> &mapPageL, const QStringList listNI, const int &rowsCounter)
{
    MyListStringList listRows;
    for(int i = 0; i < rowsCounter; i++){
        const QString ni = listNI.at(i);
        listRows.append( (listNiNotchanged.contains(ni) && mapPage.contains(ni)) ? mapPage.value(ni) : mapPageL.value(ni));
    }

    mapPage = mapPageL;
    return listRows;
}

void ZbyrMeterListMedium::createLocalSocketObject()
{


    const bool verboseMode = false;
    const bool activeDbgMessages = false;
    ZbyratorSocket *extSocket = new ZbyratorSocket(verboseMode);
    extSocket->activeDbgMessages = activeDbgMessages;

    extSocket->initializeSocket(MTD_EXT_NAME_ZBYRATOR);
    QThread *extSocketThrd = new QThread;

    extSocket->moveToThread(extSocketThrd);

    connect(this, &ZbyrMeterListMedium::killAllObjects, extSocket, &ZbyratorSocket::killAllObjects);
    connect(extSocket, SIGNAL(destroyed(QObject*)), extSocketThrd, SLOT(quit()));
    connect(extSocketThrd, SIGNAL(finished()), extSocketThrd, SLOT(deleteLater()));

//    connect(extSocket, &ZbyratorSocket::appendDbgExtData, this, &ZbyratorManager::appendDbgExtData );

    connect(extSocketThrd, &QThread::started, extSocket, &ZbyratorSocket::onThreadStarted);

    connect(extSocket, &ZbyratorSocket::onConfigChanged , this, &ZbyrMeterListMedium::onConfigChanged  );

    connect(extSocket, SIGNAL(onConfigChanged(quint16,QVariant)), this, SLOT(onReloadAllZbyratorSettingsLocalSocket()));

//    connect(extSocket, &ZbyratorSocket::command4dev     , this, &ZbyrMeterListMedium::command4devSlot);

    connect(extSocket, SIGNAL(command4dev(quint16,QString)), this, SLOT(command4devSlotLocalSocket(quint16,QString)));

    connect(this, &ZbyrMeterListMedium::command2extensionClient, extSocket, &ZbyratorSocket::command2extensionClient   );
    connect(this, &ZbyrMeterListMedium::onAboutZigBee          , extSocket, &ZbyratorSocket::sendAboutZigBeeModem      );
    connect(this, &ZbyrMeterListMedium::relayStatusChanged     , extSocket, &ZbyratorSocket::relayStatusChanged        );


//    extSocketThrd->start();
    QTimer::singleShot(7777, extSocketThrd, SLOT(start()));

}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::createPeredavatorEmbeeManager()
{
    PeredavatorCover *cover = new PeredavatorCover;
    QThread *t = new QThread;

    cover->moveToThread(t);

    connect(this, &ZbyrMeterListMedium::killAllObjects, cover, &PeredavatorCover::kickOffAllObjects);


    connect(cover, SIGNAL(destroyed(QObject*)), t, SLOT(quit()));
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
    connect(t, SIGNAL(started()), cover, SLOT(onThreadStarted()) );

    sendForcedQuickCollectDaState();
    connect(cover, &PeredavatorCover::onDirectAccessModuleReady , this, &ZbyrMeterListMedium::onQuickCollectDaObjectReady);
    connect(cover, &PeredavatorCover::onConnectionOpened        , this, &ZbyrMeterListMedium::setQuickCollectObjectState);
    connect(cover, &PeredavatorCover::onConnectionStateChanging , this, &ZbyrMeterListMedium::setQuickCollectObjectChangingState);


    connect(cover, &PeredavatorCover::giveMeIfaceSett           , this, &ZbyrMeterListMedium::sendIfaceSettings);
    connect(cover, &PeredavatorCover::append2logDirectAccess    , this, &ZbyrMeterListMedium::append2logDirectAccess);

//    connect(cover, &PeredavatorCover::showMessage                  , this, &ZbyrMeterListMedium::showMessage);
    connect(cover, &PeredavatorCover::showMessCritical          , this, &ZbyrMeterListMedium::showMessage);
    connect(cover, &PeredavatorCover::appendMess                , this, &ZbyrMeterListMedium::appendAppLog);
    connect(cover, &PeredavatorCover::appendMessHtml            , this, &ZbyrMeterListMedium::appendAppLog);
    connect(cover, &PeredavatorCover::onStatusChanged         , this, &ZbyrMeterListMedium::onQuickCollectDaStateChangedStr);

    connect(cover, &PeredavatorCover::onDasStopped              , this, &ZbyrMeterListMedium::onDasStopped);

    connect(cover, &PeredavatorCover::ifaceLogStr, this, &ZbyrMeterListMedium::ifaceLogStr);
    connect(cover, &PeredavatorCover::ifaceLogStr, this, &ZbyrMeterListMedium::ifaceLogStrFromDA);

    connect(this, &ZbyrMeterListMedium::startApiAddressator , cover, &PeredavatorCover::startApiAddressator );
    connect(this, &ZbyrMeterListMedium::stopApiAddressator  , cover, &PeredavatorCover::stopApiAddressator  );
    connect(this, &ZbyrMeterListMedium::setDaForwardNI      , cover, &PeredavatorCover::setDaForwardNI      );
    connect(this, &ZbyrMeterListMedium::setThisIfaceSett    , cover, &PeredavatorCover::setThisIfaceSettSlot    );
    connect(this, &ZbyrMeterListMedium::ifaceLogStrFromZbyrator, cover, &PeredavatorCover::ifaceLogStrFromZbyrator);


    //info
    connect(cover, &PeredavatorCover::showMessage, ucDeviceTreeW, &UCDeviceTreeWatcher::onDirectAccessServiceShowMessage);
    connect(cover, &PeredavatorCover::onStatusChanged, ucDeviceTreeW, &UCDeviceTreeWatcher::onDirectAccessServiceStatusChanged);
    connect(cover, &PeredavatorCover::onDasStarted, ucDeviceTreeW, &UCDeviceTreeWatcher::onDirectAccessServiceStateChanged);
    connect(cover, &PeredavatorCover::append2logDirectAccess, ucDeviceTreeW, &UCDeviceTreeWatcher::onDirectAccessServiceShowMessage);

    connect(ucDeviceTreeW, &UCDeviceTreeWatcher::onSetDaForwardintSettings, cover, &PeredavatorCover::onSetDaForwardintSettings);


    connect(this, &ZbyrMeterListMedium::startStopDirectAccessService, cover, &PeredavatorCover::startStopDirectAccessServiceSlot);
    connect(this, &ZbyrMeterListMedium::setUCDirectAccessServiceParams, cover, &PeredavatorCover::setUCDirectAccessServiceParams);



    resetStopDirectAccess();



    QTimer::singleShot(555, t, SLOT(start()));

}



//---------------------------------------------------------------------

void ZbyrMeterListMedium::updateRelayStatuses4meterlistExt(const QMap<QString, UCEMeterRelayStateOneRelay> &relaysttsmap)
{
    UCEMeterRelayState erelay(getTemplateValidator());
    erelay.ni2relay = relaysttsmap;

    ucDeviceTreeW->setUCEMeterRelayState(erelay);

}

//---------------------------------------------------------------------

