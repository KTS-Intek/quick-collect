#include "startpagepollv2.h"


///[!] quick-collect
#include "zbyrator-src/selectmeters4poll.h"


///[!] guisett-shared-core
//#include "src/nongui/settloader.h"

///[!] dataprocessing
#include "src/matilda/devicepollcodeselectorhelper.h"
//#include "src/matilda/viewdatahelper.h"


///[!] widgets-shared
#include "dataconcentrator-pgs/dbdataform.h"



///[!] widgets-meters
#include "dataconcetrator-pgs/templates/quickcollectstartpollparamswdgt.h"
#include "dataconcetrator-pgs/src/databasewdgtv3.h"


///[!] zbyrator-shared
#include "src/meter/meterstatehelper.h"
#include "src/meter/pollenergyhelper.h"


///[!] tasks-shared
#include "src/task/pollcodeshelper.h"


///[!] guisett-shared-core
#include "src/nongui/settloader.h"



#include "definedpollcodes.h"
#include "myucdevicetypes.h"
#include "matildaprotocolcommands.h"


//---------------------------------------------------------------------------

StartPagePollV2::StartPagePollV2(GuiHelper *gHelper, QWidget *parent) : StartQuickPollWdgt(gHelper, true, true, parent)
{
    connect(this, &StartPagePollV2::onCurrentProcessingTabKilled, this, &StartPagePollV2::onCurrentProcessingTabKilledSlot);

    addQuickPollPanel();
}



//---------------------------------------------------------------------------
QStringList StartPagePollV2::getEnrgList4code(const quint8 &code)
{
//    bool hasTariffs = false;
//    QStringList l = PollEnergyHelper::defValEnergyKeys(code);


//    if(PollCodesHelper::isAlogBookPollCode(code)){
//        l = QString("model,evnt_code,comment").split(',');
//    }else if(PollCodesHelper::isAPeriodConsumptionPollCode(code)){

//        if(PollCodesHelper::isEMeterPollCode(code)){
//            l = QString("A+ A- R+ R-").split(" ", QString::SkipEmptyParts);
//        }else if(PollCodesHelper::isWMeterPollCode(code)){
//            qDebug() << "StartPagePollV2::getEnrgList4code(const quint8 &code) no keys for this code " << code;
//        }else if(PollCodesHelper::isGMeterPollCode(code)){
//            qDebug() << "StartPagePollV2::getEnrgList4code(const quint8 &code) no keys for this code " << code;

//        }else if(PollCodesHelper::isPMeterPollCode(code)){
//            l.append(QString("chnnl dvlu").split(" ", QString::SkipEmptyParts));
//        }

//        l.prepend("intrvl");
//    }else if(PollCodesHelper::isAstatePollCode(code)){

//        l = MeterStateHelper::getEngrKeys4table();

//    }else if(PollCodesHelper::isItATariffPollCode4eMeter(code)){
//        hasTariffs = true;
//        l = QString("A+ A- R+ R-").split(" ", QString::SkipEmptyParts);

//    }else if(PollCodesHelper::isItATariffPollCode4wMeter(code)){
//        l = PollEnergyHelper::getco:: TableHeaders::getColDataWaterTotalValues().split(',');

//    }else if(PollCodesHelper::isItATariffPollCode4pMeter(code)){

//        l.append(QString("chnnl tvlu").split(" ", QString::SkipEmptyParts));


//    }else  if(PollCodesHelper::isAnInstantaneousPollCode(code)){
//        if(PollCodesHelper::isEMeterPollCode(code)){
//            l = QString("UA,UB,UC,IA,IB,IC,PA,PB,PC,QA,QB,QC,cos_fA,cos_fB,cos_fC,F").split(',')  ;
//        }else if(PollCodesHelper::isPMeterPollCode(code)){

//            l.append(QString("chnnl ivlu").split(" ", QString::SkipEmptyParts));

//        }else{
//            qDebug() << "StartPagePollV2::getEnrgList4code(const quint8 &code) no keys for this code " << code;

//        }

//    }


//    if(!hasTariffs)
//        return l;

    QStringList listEnrg = PollEnergyHelper::defValEnergyKeys(code);
//    for(int i = 0, emax = l.size(); i < 5; i++){
//        for(int e = 0; e < emax; e++)
//            listEnrg.append(QString("T%1_%2").arg(i).arg(l.at(e)));
//    }
//    listEnrg.append("stts");


    QStringList dbpreambule = QString("msec meter_sn meter_ni").split(" ");
    for(int i = 0, imax = dbpreambule.size(); i < imax && !dbpreambule.isEmpty(); i++){
        listEnrg.prepend(dbpreambule.takeLast());
    }
    listEnrg.append("stts");

    //the keys sequence is important!!!
    return listEnrg;//data base like keys, do not forget to add msec, ni and sn
}



//---------------------------------------------------------------------------

MyPollCodeList StartPagePollV2::getLvIconsAndTexts(const int &version)
{
    Q_UNUSED(version);
    MyPollCodeList l = DevicePollCodeSelectorHelper::getLvIconsAndTexts(MATILDA_PROTOCOL_VERSION_V11, DEV_POLL_EMULATOR_L2);

    for(int i = 0, imax = l.size(); i < imax; i++){
        if(l.at(i).code == UC_METER_GAS || l.at(i).code == UC_METER_UNKNOWN){
            l.removeAt(i); //ignore gas meters
            imax--;
        }
    }
    return l;


}



//---------------------------------------------------------------------------


void StartPagePollV2::createTab(const StartPollTabSettExt &sett)
{


    DbDataForm *dbData = createTemplateDataForm(sett);
    lTempPollSett.lastWdgtActive = lastWdgtAccssbltName ;
    emit onPollStarted(sett.select, lastWdgtAccssbltName,  getEnrgList4code(sett.select.pollCode), sett.deviceType);// sett.allowDate2utc);

    connect(metersListMedium, &ZbyrMeterListMedium::onUCLastReceivedDeviceRecordsQuickCollectChanged, dbData, &DbDataForm::onUCLastReceivedDeviceRecordsChangedLocal);

    //you must create some class that can process data from zbyrator to GUI
//    dbData->setStreamParameters4local(lastWdgtAccssbltName, sett.select.selectionTag);


    const MyLastSelectParamsV3 settext = DatabaseWdgtV3::fromStartPollTabSettExt(sett);

    dbData->setStreamParametersV3local(settext, lastWdgtAccssbltName);

    connect(this, &StartPagePollV2::kickOffAll, dbData, &DbDataForm::kickOffAll);

    QTimer::singleShot(1, this, SIGNAL(killSelectMeters4poll()));

    QTimer::singleShot(111, this, SLOT(saveCurrentDevSelectSett()));

}

//---------------------------------------------------------------------------

bool StartPagePollV2::canContinueWithTheseSettings(const StartPollTabSettExt &sett)
{
    Q_UNUSED(sett);
    metersListMedium->setLastPageId(accessibleName());

    return true;
}

//---------------------------------------------------------------------------

bool StartPagePollV2::createObjectsForPollAllMetersMode(const StartPollTabSettExt &selsett, QString &message)
{
    emit lockPbRead(true);

    lTempPollSett.isSelectMeters4pollProcessing = true;


    GetDataFromDbDoneSignalizator *signalizator = new GetDataFromDbDoneSignalizator(this);
    connect(signalizator, SIGNAL(onDbSelectorKickedOff()), this, SLOT(onSelectMeters4pollKickedOff()));


    SelectMeters4poll *w = new SelectMeters4poll(signalizator, gHelper,  this);
    connect(this, SIGNAL(killSelectMeters4poll()), w, SLOT(deleteLater()) );
//    connect(w, SIGNAL(onReloadAllMeters()), this, SIGNAL(onReloadAllMeters()) );
    connect(w, &SelectMeters4poll::onReloadAllMeters, metersListMedium, &ZbyrMeterListMedium::sendAllMeters);

    connect(w, SIGNAL(command4dev(quint16,QVariantMap)), this, SLOT(command4devSlot(quint16,QVariantMap)));
    connect(this, SIGNAL(command4dev(quint16,QString)), w, SLOT(deleteLater()) );
    connect(this, SIGNAL(command4dev(quint16,QVariantMap)), w, SLOT(deleteLater()) );

    connect(metersListMedium, SIGNAL(onAllMeters(UniversalMeterSettList)), w, SIGNAL(onAllMeters(UniversalMeterSettList)) );

    const QDateTime dtFrom = QDateTime::fromMSecsSinceEpoch(selsett.select.timeRange.msecFrom);// selsett.hashSelSett.value("FromDT").toDateTime();
    const QDateTime dtTo = (selsett.select.timeRange.msecTo <= selsett.select.timeRange.msecFrom)
            ? QDateTime() :  QDateTime::fromMSecsSinceEpoch(selsett.select.timeRange.msecTo);// selsett.hashSelSett.value("ToDT").toDateTime();
    const quint8 pollCode = selsett.select.pollCode;


    switch(lastSelsett.deviceType){

    case UC_METER_ELECTRICITY:{

        w->setPollSettElectric(dtFrom, dtTo, pollCode, getIgnoreRetries());
        break;}

    case UC_METER_WATER      :{
        w->setPollSettWater(dtFrom, dtTo, pollCode, lastWtrSett.sendSleepCommand, lastWtrSett.secs, lastWtrSett.checkProfile, getIgnoreRetries());
        break;}

//    case UC_METER_GAS      :{
//        w->setPollSettGas(dtFrom, dtTo, pollCode, getIgnoreRetries());
//        break;}

    case UC_METER_PULSE      :{
        w->setPollSettPulse(dtFrom, dtTo, pollCode, getIgnoreRetries());
        break;}


    default: qDebug() << "can't set pollSett StartPagePoll lastSelsett.deviceType=" << lastSelsett.deviceType;
        message = tr("Couldn't start the poll. The unknown device type '%1'").arg(int(lastSelsett.deviceType)); return false;
    }
    emit addWdgt2stackWdgt(w, WDGT_TYPE_ZBYR_SELECT_METERS4POLL, true, tr("Select"), ":/katynko/svg/dialog-ok-apply.svg");

    return true;
}

OneDevicePollCodes StartPagePollV2::getDeviceSelectSett4adev(const int &devtype)
{
    OneDevicePollCodes codes;

    bool isUnknownDevType = false;
    switch(devtype){
    case UC_METER_ELECTRICITY   : codes = DevicePollCodeSelectorHelper::getElectricityMetersSelectSett(false)   ; break;
    case UC_METER_WATER         : codes = DevicePollCodeSelectorHelper::getWaterMetersSelectSett()              ; break;
    case UC_METER_GAS           : codes = DevicePollCodeSelectorHelper::getGasMetersSelectSett()                ; break;
    case UC_METER_PULSE         : codes = DevicePollCodeSelectorHelper::getPulseMetersSelectSett()              ; break;
    case UC_METER_UNKNOWN       : codes = DevicePollCodeSelectorHelper::getAllDevicesSelectSett()               ; break;
    default: isUnknownDevType = true; break;
    }
///home/hello_zb/My_Prog/Qt5_progs/TESTS/Matilda-units/gui/guisett-shared-core/src/nongui/pollcodesoperations.h
///
///
    if(!isUnknownDevType){

        const QString objectid = "EMULQP";// gHelper->ucDeviceTreeW->getCachedUCAboutDeviceSettings().objectId;
        if(objectid.isEmpty()){
            return PollCodesOperations::fromVariantHash(SettLoader().loadSett(SETT_PAGES_DB_SELSETTLST).toHash(), codes);
        }


        const QVariantHash oneobjid = SettLoader().loadSett(SETT_PAGES_DB_SELSETT).toHash().value(objectid).toHash();
        if(oneobjid.isEmpty())
            return PollCodesOperations::fromVariantHash(SettLoader().loadSett(SETT_PAGES_DB_SELSETTLST).toHash(), codes);


        return PollCodesOperations::fromVariantHash(oneobjid, codes);

//        return codes;
    }

    codes.currentPollCode = 0;
    codes.pollcodesetts.insert(0, OnePollCodeSett());
    codes.pollcodesnames.append(PollCodeName2data("name", 0, "iconstr"));
    //    codes.datetimesett = SelectDateTimeSett() is is already ready for using

    return codes;
}

bool StartPagePollV2::getIgnoreRetries()
{
    return myLastCbxState.ignoreRetries;
}
//---------------------------------------------------------------------------
void StartPagePollV2::checkHasReadWriteButtons()
{
    setHasReadButton(true);
}

//---------------------------------------------------------------------------

void StartPagePollV2::disconnectMeFromAppendData()
{
    DbDataForm *f = qobject_cast<DbDataForm*>(QObject::sender());
    if(f){
        try{
            f->accessibleName();
        }catch(...){
            return;
        }
        disconnect(this, SIGNAL(appendData2model(QString,QVariantHash)), f, SIGNAL(appendData2model(QString,QVariantHash)) );
        disconnect(this, SIGNAL(appendData2model(QString,QVariantHash)), f, SIGNAL(appendEvData2model(QString,QVariantHash)) );

        disconnect(f, SIGNAL(disconnectMeFromAppendData()), this, SLOT(disconnectMeFromAppendData()) );
        disconnect(gHelper, SIGNAL(setPbReadEnableDisable(bool)), f, SIGNAL(checkBlncStateEnNow(bool)) );

    }
}

//---------------------------------------------------------------------------

void StartPagePollV2::command4devSlot(quint16 command, QVariantMap map)
{
    createTab(lastSelsett);
    emit command4dev(command, map);
    //    SettLoader::saveSett(SETT_ZBRTR_POLLPAGE_MODE, ui->tbwTypeOfMeter->currentIndex());
}

//---------------------------------------------------------------------------

void StartPagePollV2::onSelectMeters4pollKickedOff()
{
    lTempPollSett.isSelectMeters4pollProcessing = false;
    emit lockPbRead(lTempPollSett.isSelectMeters4pollProcessing);

}

//---------------------------------------------------------------------------

void StartPagePollV2::onCurrentProcessingTabKilledSlot()
{
    if(lTempPollSett.lastWdgtActive.isEmpty())
        return;

    emit killCurrentTask();


}

void StartPagePollV2::saveCurrentDevSelectSett()
{
    const auto onedev = getCurrentDevOneDevicePollCodes();

    const QVariantHash h = PollCodesOperations::fromOneDevicePollCodes(onedev);

    SettLoader().saveSett(SETT_PAGES_DB_SELSETTLST, h);

    const QString objectid = "EMULQP";// gHelper->ucDeviceTreeW->getCachedUCAboutDeviceSettings().objectId;

    if(!objectid.isEmpty()){
        QVariantHash manyobjects = SettLoader().loadSett(SETT_PAGES_DB_SELSETT).toHash();
        manyobjects.insert(objectid, h);
        SettLoader().saveSett(SETT_PAGES_DB_SELSETT, manyobjects);
    }


    hideLblMessage();
}

//---------------------------------------------------------------------------

void StartPagePollV2::addQuickPollPanel()
{
    startTab->disconnectLblWaitTxtDataBase();

    QuickCollectStartPollParamsWdgt *w = new QuickCollectStartPollParamsWdgt(this);
    startTab->addTopWidget(w);

    connect(startTab, &SelectFromDatabaseTemplateStartTab::setLblWaitTxtDatabase, w, &QuickCollectStartPollParamsWdgt::setLblWaitTxtDatabase);
    connect(w, &QuickCollectStartPollParamsWdgt::onCbxIgnoreRetr, this ,&StartPagePollV2::onCbxIgnoreRetr);
    connect(w, &QuickCollectStartPollParamsWdgt::onCbxOnlyGlobalConnection, this, &StartPagePollV2::onCbxOnlyGlobalConnection);

    connect(w, &QuickCollectStartPollParamsWdgt::onCbxIgnoreRetr, [=](bool checked){
       myLastCbxState.ignoreRetries = checked;
    });

    connect(w, &QuickCollectStartPollParamsWdgt::onCbxOnlyGlobalConnection, [=](bool checked){
       myLastCbxState.useGlobalConnection = checked;
    });

    connect(this, &StartPagePollV2::requestToSwitchIgnoreCycles, w, &QuickCollectStartPollParamsWdgt::requestToSwitchIgnoreCycles);
    QTimer::singleShot(3333, w, SLOT(sendYourCbxStates()));

}

//---------------------------------------------------------------------------
