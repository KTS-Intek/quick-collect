#include "zbyrmeterlistmedium.h"


#include <QDebug>
#include <QTimer>


///[!] guisett-shared-core
#include "src/nongui/tableheaders.h"
#include "src/nongui/settloader.h"


///[!] device-poll
#include "src/meter/zbyratorfilesetthelper.cpp"


///[!] quick-collect
#include "zbyrator-src/src/zbyrtableheaders.h"
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



//---------------------------------------------------------------------

void ZbyrMeterListMedium::onAllMetersSlot(UniversalMeterSettList allMeters)
{
    if(allMeters.isEmpty())
        return;

    UCEMeterSettings emeter;
    UCWMeterSettings wmeter;


    QMap<quint8, UniversalMeterSettList> map2meters;
    for(int i = 0, imax = allMeters.size(); i < imax; i++){
        const UniversalMeterSett m = allMeters.at(i);

        switch(m.deviceType){
        case UC_METER_ELECTRICITY   : emeter.eMeterContainer.append(universalMeterSett2emeterSettings(m))  ; break;
        case UC_METER_WATER         : wmeter.wMeterContainer.append(universalMeterSett2wmeterSettings(m)); break;
        }

        if(true){
            UniversalMeterSettList l = map2meters.value(m.deviceType);
            l.append(m);
            map2meters.insert(m.deviceType, l);
        }

    }



    emeter.validator = wmeter.validator = getTemplateValidator();

    ucDeviceTreeW->setUCEMeterSettings(emeter);
    ucDeviceTreeW->setUCWMeterSettings(wmeter);



    const QList<quint8> lk = map2meters.keys();
    for(int i = 0, imax = lk.size(); i < imax; i++)
        emit onAddMeters(lk.at(i), map2meters.value(lk.at(i)), MyNi2model(), true);

    onElectricitylistOfMeters(allMeters, MyNi2model(), true);
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::meterElectricityModelChanged(QVariantList meters)
{
    emit onConfigChanged(MTD_EXT_CUSTOM_COMMAND_2, true);
    lastSaveMeterList.lastMeterList = meters;
    lastSaveMeterList.deviceType = UC_METER_ELECTRICITY;
    emit startTmrSaveLater();

}

void ZbyrMeterListMedium::meterWaterModelChanged(QVariantList meters)
{
    emit onConfigChanged(MTD_EXT_CUSTOM_COMMAND_2, true);



    lastSaveMeterList.lastMeterList = meters;
    lastSaveMeterList.deviceType = UC_METER_WATER;
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
    emit onAddMeters(deviceType, activeMeters, switchedOffMeters, false);
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


    connect(this, &ZbyrMeterListMedium::onAddMeters     , c, &ZbyratorDataCalculation::onAlistOfMeters  );
    connect(this, &ZbyrMeterListMedium::appendMeterData , c, &ZbyratorDataCalculation::appendMeterData  );
    connect(this, &ZbyrMeterListMedium::onPollStarted   , c, &ZbyratorDataCalculation::onPollStarted    );
    connect(this, &ZbyrMeterListMedium::onUconStartPoll , c, &ZbyratorDataCalculation::onUconStartPoll  );

    connect(this, &ZbyrMeterListMedium::onMeterPollCancelled, c, &ZbyratorDataCalculation::onMeterPollCancelled);

    connect(c, &ZbyratorDataCalculation::appendData2model, this, &ZbyrMeterListMedium::appendData2model);
//    connect(c, &ZbyratorDataCalculation::setLblWaitTxt   , this, &ZbyrMeterListMedium::setLblWaitTxt);
    connect(c, &ZbyratorDataCalculation::updateHashSn2meter, this, &ZbyrMeterListMedium::updateHashSn2meter);

    connect(c, &ZbyratorDataCalculation::setCOMMAND_READ_POLL_STATISTIC, this, &ZbyrMeterListMedium::setStatisticOfExchangePageSett);
    connect(this, &ZbyrMeterListMedium::onCOMMAND_READ_POLL_STATISTIC, c, &ZbyratorDataCalculation::onCOMMAND_READ_POLL_STATISTIC);

    connect(this, &ZbyrMeterListMedium::setVirtualMetersSett, c, &ZbyratorDataCalculation::setVirtualMetersSett);

    t->start();

}

void ZbyrMeterListMedium::createDatabaseMedium()
{
    ZbyratorDatabaseMedium *m = new ZbyratorDatabaseMedium;
    QThread *t = new QThread;
    m->moveToThread(t);

    connect(this, &ZbyrMeterListMedium::killAllObjects, m, &ZbyratorDatabaseMedium::kickOffObject);


    connect(m, SIGNAL(destroyed(QObject*)), t, SLOT(quit()) );
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));

    connect(t, SIGNAL(started()), m, SLOT(onThreadStarted()) );
    connect(this, &ZbyrMeterListMedium::onAddMeters     , m, &ZbyratorDatabaseMedium::onAlistOfMeters);


    connect(this, SIGNAL(data2dbMedium(quint16,QVariant)), m, SLOT(data2matilda4inCMD(quint16,QVariant)));
    connect(this, SIGNAL(stopReadDatabase()), m, SLOT(stopOperationSlot()) );

    connect(this, &ZbyrMeterListMedium::setDateMask, m, &ZbyratorDatabaseMedium::setDateMask);
    connect(this, &ZbyrMeterListMedium::setDotPos, m, &ZbyratorDatabaseMedium::setDotPos);

    connect(m, SIGNAL(setPbReadEnableDisable(bool)), this, SIGNAL(setPbReadEnableDisable(bool)));

    connect(m, SIGNAL(appendDataDatabase(QVariantHash)), this, SIGNAL(appendDataDatabase(QVariantHash)));
    connect(m, SIGNAL(appendDataDatabaseMJ(QVariantHash)), this, SIGNAL(appendDataDatabaseMJ(QVariantHash)));

    connect(m, SIGNAL(setLblWaitTxtDatabase(QString)), this, SIGNAL(setLblWaitTxtDatabase(QString)));
    connect(m, SIGNAL(setLblWaitTxtDatabaseMj(QString)), this, SIGNAL(setLblWaitTxtDatabaseMj(QString)));

    connect(this, &ZbyrMeterListMedium::setVirtualMetersSett, m, &ZbyratorDatabaseMedium::setVirtualMetersSett);

    t->start();


}

void ZbyrMeterListMedium::onAllStatHash(QStringList allstat)
{
    liststat = allstat;
}
//---------------------------------------------------------------------
void ZbyrMeterListMedium::onPollCodeChangedStat(QVariantHash hash)
{
    const QString pollStr = hash.value("c").toString();
    QString niStr = hash.value("n").toString();
    const int mode = hash.value("m", -1).toInt();


    if(mode < 0)
        niStr.clear();

    QStringList rezListStr;

    if(niStr.contains("'"))
        niStr.replace("'", " ");

    QRegExp regExp(niStr, Qt::CaseInsensitive, QRegExp::RegExp);

    const QStringList ls = liststat;
    if(!ls.isEmpty())
        rezListStr.append(ls.first());




//    QHash<QString, OneModemStat> hNi2allData;
//    QStringList lk2niAllData;

    for(int i = 1, iMax = ls.size(); i < iMax; i++){
        if(ls.at(i).split("\t", QString::SkipEmptyParts).isEmpty())
            continue;

        const QString niLine = ls.at(i).split("\t", QString::SkipEmptyParts).first();

        if(niStr.isEmpty() || (mode == 0 && niLine == niStr) || (mode == 1 && niLine.contains(regExp))){
            //    QString retVal(tr("Modem NI\tModel\tPoll Code\tTime\tMess. Count\tWrite/Read byte\tElapsed (s)\tLast retry\n"));
            if(pollStr.isEmpty()){
                rezListStr.append(ls.at(i));
            }else{
                const QStringList l = ls.at(i).split("\t", QString::SkipEmptyParts);
                if(l.size() < 5)
                    continue;
                if(l.at(2) == pollStr)
                    rezListStr.append(ls.at(i));

            }
        }
    }
    emit onCOMMAND_READ_POLL_STATISTIC(rezListStr);
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
    case 1: emit waterMeterSchedulerStts(ni, dtLocal, stts, QVariantHash(), ""); break;
    case 2: meterRelayStatus(ni, dtLocal,  RELAY_STATE_UNKN, RELAY_STATE_UNKN); break; //relay
    case 5: emit meterDateTimeDstStatus(ni, dtLocal, stts); break;
    }

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
void ZbyrMeterListMedium::sendCachedDataAboutRelays(const QStringList &niswithoutsttses)
{
    QString errmess;
    const QMap<QString,UCEMeterRelayStateOneRelay> relaysttsmap = LastMetersStatusesManager::getLastRelayStatusesMap(errmess);


    for(int i = 0, imax = niswithoutsttses.size(); i < imax; i++){
        const UCEMeterRelayStateOneRelay stts = relaysttsmap.value(niswithoutsttses.at(i));
//        if(!stts.dtLocal.isValid())
//            continue;

//        emit meterRelayStatus(niswithoutsttses.at(i), stts.dtLocal, stts.mainstts, stts.secondarystts);

    }
//    emit startTmrUpdateRelayStatuses(1111);



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
        lrelay.hasRequestFromMeterList = true;
        break;}//realy operations, main relay
    case COMMAND_READ_ELMTRRELAY_TABLE:{
        lrelay.hasRequestFromMeterList = true;
//        emit startTmrUpdateRelayStatuses(111);

        break;} //send last realy statuses
    }

    qDebug() << "onElMeterRelayChanged mWrite2RemoteDev " << lrelay.hasRequestFromMeterList << command;

}

//---------------------------------------------------------------------
void ZbyrMeterListMedium::updateRelayStatuses4meterlist()
{



    QString errmessage;
    const QMap<QString,UCEMeterRelayStateOneRelay> relaysttsmap = LastMetersStatusesManager::getLastRelayStatusesMap(errmessage);

    qDebug() << "onElMeterRelayChanged updateRelayStatuses4meterlist " << relaysttsmap.size() << errmessage;

    updateRelayStatuses4meterlistExt(relaysttsmap);
}

void ZbyrMeterListMedium::setPbWriteDis(bool disabled)
{
    pbWriteDis = disabled;
}

void ZbyrMeterListMedium::command2extension(quint16 extName, quint16 command, QVariant data)
{
    qDebug() << "Extname " << extName << command;
    emit onConfigChanged(command, data);
    onReloadAllZbyratorSettingsLocalSocket();
}


void ZbyrMeterListMedium::onGetUCEMeterSettings(QString senderName)
{
//    Q_UNUSED(senderName);
    onGetUCSupportedMetersInfo(senderName);
    QTimer::singleShot(111, this, SIGNAL(onReloadAllMeters2zbyrator()));
}

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

void ZbyrMeterListMedium::onGetUCEMeterRelayState(QString senderName)
{
    Q_UNUSED(senderName);
    QTimer::singleShot(111, this, SLOT(updateRelayStatuses4meterlist()));
}

void ZbyrMeterListMedium::meterRelayStatus(QString ni, QDateTime dtLocal, quint8 mainstts, quint8 secondarystts)
{
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

void ZbyrMeterListMedium::onGetUCSupportedMetersInfo(QString senderName)
{

    Q_UNUSED(senderName);
    UCSupportedMetersInfo emeter = ucDeviceTreeW->getCachedUCSupportedMetersInfoElectricity();
    UCSupportedMetersInfo wmeter = ucDeviceTreeW->getCachedUCSupportedMetersInfoWater();

    if(emeter.validator.dtlastupdate.isValid() && wmeter.validator.dtlastupdate.isValid())
        return;

    emeter = wmeter = UCSupportedMetersInfo();//reset all


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
        case UC_METER_ELECTRICITY: emeter.pluginNames.append(plgname); emeter.pluginParams.insert(plgname, plgparam); break;
        case UC_METER_WATER: wmeter.pluginNames.append(plgname); wmeter.pluginParams.insert(plgname, plgparam); break;

        }
    }


    emeter.validator = wmeter.validator = getTemplateValidator();

    ucDeviceTreeW->setUCSupportedMetersInfoEMeter(emeter);
    ucDeviceTreeW->setUCSupportedMetersInfoWMeter(wmeter);


}

void ZbyrMeterListMedium::onGetUCWMeterSettings(QString senderName)
{
    onGetUCEMeterSettings(senderName);//it reloads all meters;
}

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


    QMap<QString, QStringList> relayPageL, dateTimePageL, waterProfiles;
    LastList2pages lastElectricityMeters2pagesL;

    LastList2pages lastWaterMeters2pagesL;

    LastList2pages lastMeters2pagesL;

    int pollOnMeters = 0;
    int pollOffMeters = switchedOffMetersSize;

    int meterElectricityActive = 0;
    int meterWaterActive = 0;
//    QStringList listNiNotchanged;

    QMap<quint8, QString> mapMetertype2ni;

    QStringList meternisWithoutRelaySttses;
    QStringList allElectricMeters;

    for(int i = 0; i < activeMetersSize; i++){

        const UniversalMeterSett m = activeMeters.at(i);



        switch(m.deviceType){
        case UC_METER_ELECTRICITY:{
            allElectricMeters.append(m.ni);
            break;}

        case UC_METER_WATER:{


            break;}
        }

        if(checkOffMeters && !m.pollEnbl){
            pollOffMeters++;
            continue;
        }
        pollOnMeters++;
        const QString mainParms = QString("%1\t%2\t%3\t%4\t%5").arg(m.model).arg(m.version).arg(m.sn).arg(m.memo).arg(m.coordinate);//model version SN memo



        switch(m.deviceType){
        case UC_METER_ELECTRICITY:{
            meterElectricityActive++;
            lastElectricityMeters2pagesL.listNI.append(m.ni);
            lastElectricityMeters2pagesL.mainParams.append(mainParms);
            relayPageL.insert(m.ni, addRelayRow(m));
            meternisWithoutRelaySttses.append(m.ni);
            break;}

        case UC_METER_WATER:{
            meterWaterActive++;
            lastWaterMeters2pagesL.listNI.append(m.ni);
            lastWaterMeters2pagesL.mainParams.append(mainParms);
            waterProfiles.insert(m.ni, addWaterProfileRow(m));


            break;}
        }


        lastMeters2pagesL.listNI.append(m.ni);
        lastMeters2pagesL.mainParams.append(mainParms);
        dateTimePageL.insert(m.ni, addDateTimeRow(m));


//        if(lastMeters2pages.mainParams.contains(mainParms))
//            listNiNotchanged.append(m.ni);




        mapMetertype2ni.insertMulti(m.deviceType, m.ni);

//        if(m.ni == UC_METER_ELECTRICITY)

    }

    if(!allElectricMeters.isEmpty()){
        lrelay.meternis = allElectricMeters;
//        emit startTmrUpdateRelayStatuses(3333);
    }

//    if(mapMeters2pages.contains("relay") && mapMeters2pages)

    if(metersChanged(mapMeters2pages, "Relay", lastElectricityMeters2pagesL))
        emit setRelayPageSett(getRowsList(relayPage, QStringList(), relayPageL, lastElectricityMeters2pagesL.listNI, meterElectricityActive), QVariantMap(), ZbyrTableHeaders::getRelayPageHeader(), StandardItemModelHelper::getHeaderData(ZbyrTableHeaders::getRelayPageHeader().size()), true);
    else
        meternisWithoutRelaySttses.clear();



    if(metersChanged(mapMeters2pages, "Date and time", lastMeters2pagesL))
        emit setDateTimePageSett(getRowsList(dateTimePage, QStringList(), dateTimePageL, lastMeters2pagesL.listNI, pollOnMeters), QVariantMap(), ZbyrTableHeaders::getMeterDateTimePageHeader(), StandardItemModelHelper::getHeaderData(8), true);


    if(metersChanged(mapMeters2pages, "Scheduler for water meters", lastWaterMeters2pagesL)){
//        emit setWaterMeterSchedulerPageSett(getRowsList(waterSchedulerPage, QStringList(), waterProfiles, lastWaterMeters2pagesL.listNI, meterWaterActive), QVariantMap(), TableHeaders::getColNames4wtrLastProfile().split(","),
//                                            StandardItemModelHelper::getHeaderData(TableHeaders::getColNames4wtrLastProfile().split(",").size()), true);
        QTimer::singleShot(11, this, SIGNAL(reloadSavedSleepProfiles()));
    }



//    if(checkOffMeters || lastMeters2pages.listNI != lastMeters2pagesL.listNI || lastMeters2pages.mainParams != lastMeters2pagesL.mainParams){


//        emit setDateTimePageSett(getRowsList(dateTimePage, listNiNotchanged, dateTimePageL, lastMeters2pagesL.listNI, pollOnMeters), QVariantMap(), ZbyrTableHeaders::getMeterDateTimePageHeader(), StandardItemModelHelper::getHeaderData(8), true);

//        lastMeters2pages.listNI = lastMeters2pagesL.listNI;
//        lastMeters2pages.mainParams = lastMeters2pagesL.mainParams;

//    }

    emit onUpdatedSavedList(pollOnMeters, pollOffMeters, meterElectricityActive, meterWaterActive);

    if(!meternisWithoutRelaySttses.isEmpty())
        sendCachedDataAboutRelays(meternisWithoutRelaySttses);

}

//---------------------------------------------------------------------

QStringList ZbyrMeterListMedium::addRelayRow(const UniversalMeterSett &m)
{
    QStringList relayRow;

        relayRow.append("");//datetime
        relayRow.append("");//relay
        relayRow.append("");//second relay

    relayRow.append(m.version.isEmpty() ? m.model : QString("%1, %2").arg(m.model).arg(m.version));
    relayRow.append(m.sn);
    relayRow.append(m.ni);
    relayRow.append(m.memo);
    relayRow.append(m.coordinate);
    return relayRow;
}

//---------------------------------------------------------------------

QStringList ZbyrMeterListMedium::addDateTimeRow(const UniversalMeterSett &m)
{
    QStringList dateTimeRow;
    dateTimeRow.append("");//datetime PC
    dateTimeRow.append("");//dateTime Meter
    dateTimeRow.append(m.version.isEmpty() ? m.model : QString("%1, %2").arg(m.model).arg(m.version));
    dateTimeRow.append(m.sn);
    dateTimeRow.append(m.ni);
    dateTimeRow.append(m.memo);
    dateTimeRow.append("");//DST profile
    dateTimeRow.append(m.coordinate);
    return dateTimeRow;

}

QStringList ZbyrMeterListMedium::addWaterProfileRow(const UniversalMeterSett &m)
{
    QStringList dateTimeRow;
    dateTimeRow.append("");//time
    dateTimeRow.append("");//profile
    dateTimeRow.append(m.version.isEmpty() ? m.model : QString("%1, %2").arg(m.model).arg(m.version));
    dateTimeRow.append(m.sn);
    dateTimeRow.append(m.ni);
    dateTimeRow.append(m.memo);
    dateTimeRow.append(m.coordinate);
    dateTimeRow.append("");//source
    return dateTimeRow;
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

    connect(cover, &PeredavatorCover::showMess                  , this, &ZbyrMeterListMedium::showMessage);
    connect(cover, &PeredavatorCover::showMessCritical          , this, &ZbyrMeterListMedium::showMessage);
    connect(cover, &PeredavatorCover::appendMess                , this, &ZbyrMeterListMedium::appendAppLog);
    connect(cover, &PeredavatorCover::appendMessHtml            , this, &ZbyrMeterListMedium::appendAppLog);
    connect(cover, &PeredavatorCover::onStateChangedStr         , this, &ZbyrMeterListMedium::onQuickCollectDaStateChangedStr);
    connect(cover, &PeredavatorCover::onDasStopped              , this, &ZbyrMeterListMedium::onDasStopped);
    connect(cover, &PeredavatorCover::onDasStarted              , this, &ZbyrMeterListMedium::onDasStarted);

    connect(cover, &PeredavatorCover::ifaceLogStr, this, &ZbyrMeterListMedium::ifaceLogStr);

    connect(this, &ZbyrMeterListMedium::startApiAddressator , cover, &PeredavatorCover::startApiAddressator );
    connect(this, &ZbyrMeterListMedium::stopApiAddressator  , cover, &PeredavatorCover::stopApiAddressator  );
    connect(this, &ZbyrMeterListMedium::setDaForwardNI      , cover, &PeredavatorCover::setDaForwardNI      );
    connect(this, &ZbyrMeterListMedium::setThisIfaceSett    , cover, &PeredavatorCover::setThisIfaceSett    );
    connect(this, &ZbyrMeterListMedium::ifaceLogStrFromZbyrator, cover, &PeredavatorCover::ifaceLogStrFromZbyrator);


    QTimer::singleShot(555, t, SLOT(start()));

}



//---------------------------------------------------------------------

void ZbyrMeterListMedium::updateRelayStatuses4meterlistExt(const QMap<QString, UCEMeterRelayStateOneRelay> &relaysttsmap)
{
    UCEMeterRelayState erelay(getTemplateValidator());
    erelay.ni2relay = relaysttsmap;

    ucDeviceTreeW->setUCEMeterRelayState(erelay);

//    QVariantHash lastMeterRelay;
//    const QString mask = dateMask + " hh:mm:ss";
////    const qint64 currmsec = QDateTime::currentMSecsSinceEpoch();
//    const QStringList niswithoutsttses = lrelay.meternis;

//    qDebug() << "onElMeterRelayChanged updateRelayStatuses4meterlistExt " << niswithoutsttses.size() << lrelay.hasRequestFromMeterList;

//    if(niswithoutsttses.isEmpty())
//        return;//nothing to update

//    for(int i = 0, imax = niswithoutsttses.size(); i < imax; i++){
//        const LastMetersStatusesManager::MyMeterRelayStatus stts = relaysttsmap.value(niswithoutsttses.at(i));
//        if(!stts.dtLocal.isValid())
//            continue;


//        const qint64 msec = stts.dtLocal.toMSecsSinceEpoch();
//        QString dt = QDateTime::fromMSecsSinceEpoch(msec).toLocalTime().toString(mask);

//        const QString ni = niswithoutsttses.at(i);
//        const quint16 mainstts = stts.mainstts;
//        const quint16 secondarystts = stts.secondarystts;



//        QVariantMap map;
//        map.insert("stts", RelayStateHelper::getRelayStatusHuman(mainstts));
//        map.insert("dt", dt);
//        map.insert("msec", msec);
//        map.insert("ico", MeterStateHelper4gui::getRelayIcostr4status(mainstts));

//        map.insert("stts2", RelayStateHelper::getRelayStatusHuman(secondarystts));
//        map.insert("ico2", MeterStateHelper4gui::getRelayIcostr4status(secondarystts));




//        lastMeterRelay.insert(ni, map);

//    }


//    if(true){
//        QVariantMap map;
//        map.insert("ico", ":/katynko/svg3/relay-load-unknown.svg");
//        lastMeterRelay.insert("\r\ndefault\r\n", map);
//    }


//    if(lrelay.lastMeterRelay != lastMeterRelay || lrelay.hasRequestFromMeterList){
//        lrelay.lastMeterRelay = lastMeterRelay;
//        lrelay.hasRequestFromMeterList = false;
//        emit onElMeterRelayChanged(lrelay.lastMeterRelay);
//    }
}

//---------------------------------------------------------------------

