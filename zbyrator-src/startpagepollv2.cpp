#include "startpagepollv2.h"


///[!] quick-collect
#include "zbyrator-src/src/startpollsetmetermodeldialog.h"
#include "zbyrator-src/selectmeters4poll.h"


///[!] guisett-shared-core
//#include "src/nongui/settloader.h"

///[!] dataprocessing
#include "src/matilda/devicepollcodeselectorhelper.h"
#include "src/matilda/viewdatahelper.h"


///[!] widgets-shared
#include "dataconcentrator-pgs/dbdataform.h"


///[!] zbyrator-shared
#include "src/zbyrator-v2/quickpollhelper.h"
#include "src/meter/meterstatehelper.h"



#include "definedpollcodes.h"
#include "myucmmeterstypes.h"
#include "matildaprotocolcommands.h"


//---------------------------------------------------------------------------

StartPagePollV2::StartPagePollV2(GuiHelper *gHelper, QWidget *parent) : SelectFromDatabaseTemplate(gHelper, true, parent)
{
    setPbReadStopVisible(false);

    connect(this, &StartPagePollV2::pageEndInit, this, &StartPagePollV2::getMetersLater);
    connect(this, &StartPagePollV2::lockButtons, this, &StartPagePollV2::lockPushbuttonsRead);
    connect(this, &StartPagePollV2::lockButtons, this, &StartPagePollV2::onLockButtons);
    connect(this, &StartPagePollV2::onCurrentProcessingTabKilled, this, &StartPagePollV2::onCurrentProcessingTabKilledSlot);

}

//---------------------------------------------------------------------------

QString StartPagePollV2::getArgsElectricMeter(const QStringList &nis, const QDateTime &dtTo, const QDateTime &dtFrom, QString &mess)
{
    const int tariff = 4;
    const QString enrgline = QString("A+ A- R+ R-");
    return QuickPollHelper::createQuickPollLineSmpl(UC_METER_ELECTRICITY, getOneLineModelNiPas(UC_METER_ELECTRICITY, nis),
                                                    "", ""
                                                    , enrgline, tariff, dtTo, dtFrom, true, mess);
}

//---------------------------------------------------------------------------

QString StartPagePollV2::getArgsWaterMeter(const QStringList &nis, const QDateTime &dtTo, const QDateTime &dtFrom, QString &mess)
{
    openWtrSleepDlg();
    if(!lastWtrSett.isValid)
        return QString();

    int secs2sleep = lastWtrSett.sendSleepCommand ? lastWtrSett.secs : 0;
    const int tariff = 4;
    return QuickPollHelper::createQuickPollLine(UC_METER_WATER, getOneLineModelNiPas(UC_METER_WATER, nis),
                                                "", ""
                                                , "", tariff, dtTo, dtFrom, true, secs2sleep, lastWtrSett.checkProfile, mess);
}

//---------------------------------------------------------------------------

QString StartPagePollV2::getOneLineModelNiPas(const quint8 &meterType, const QStringList &nis)
{

    QStringList models;
    switch(meterType){
    case UC_METER_ELECTRICITY   : models = lDevInfo->matildaDev.meterModels     ; break;
    case UC_METER_WATER         : models = lDevInfo->matildaDev.wtrMeterModels  ;break;
    }

    StartPollSetMeterModelDialog *d = new StartPollSetMeterModelDialog(gHelper->mapDevId2memo.value(meterType).hashMeterNi2info, nis, models, this);
    connect(d, &StartPollSetMeterModelDialog::setNi2model, this, &StartPagePollV2::setNi2model);
    d->exec();

    d->deleteLater();
    //        lDevInfo->matildaDev.meterModels = lModel;


    return lTempPollSett.lastLineModelNiPasswd;
}
//---------------------------------------------------------------------------
QStringList StartPagePollV2::getEnrgList4code(const quint8 &code)
{
    bool hasTariffs = false;
    QStringList l;
    switch(code){
    case POLL_CODE_READ_TOTAL       :
    case POLL_CODE_READ_END_DAY     :
    case POLL_CODE_READ_END_MONTH   : hasTariffs = true;
    case POLL_CODE_READ_POWER       : l = QString("intrvl A+ A- R+ R-").split(" ", QString::SkipEmptyParts); break;

    case POLL_CODE_WTR_METER_STATE  :
    case POLL_CODE_READ_METER_STATE : l = MeterStateHelper::getEngrKeys4table(); break;// listEnrg = QString("relay,deg,vls,prm").split(","); break;
    case POLL_CODE_READ_VOLTAGE     : l = QString("UA,UB,UC,IA,IB,IC,PA,PB,PC,QA,QB,QC,cos_fA,cos_fB,cos_fC,F").split(',')  ; break;

    case POLL_CODE_WTR_METER_LOGBOOK:
    case POLL_CODE_READ_METER_LOGBOOK:
    case POLL_CODE_MATILDA_EVNTS    : l = QString("model,evnt_code,comment").split(','); break;


    case POLL_CODE_WTR_TOTAL        :
    case POLL_CODE_WTR_END_DAY      :
    case POLL_CODE_WTR_END_MONTH    : l = TableHeaders::getColDataWaterTotalValues().split(','); break;
    }


    if(!hasTariffs)
        return l;

    QStringList listEnrg;
    for(int i = 0, emax = l.size(); i < 5; i++){
        for(int e = 1; e < emax; e++)//ignore intrvl
            listEnrg.append(QString("T%1_%2").arg(i).arg(l.at(e)));
    }
//    listEnrg.append("stts");
    return listEnrg;
}

//---------------------------------------------------------------------------

QString StartPagePollV2::getTabTitle()
{
    return tr("Start a poll");

}

//---------------------------------------------------------------------------

QString StartPagePollV2::getTabIconPath()
{
    return ":/katynko/svg/edit-table-insert-row-below.svg";

}

//---------------------------------------------------------------------------

LvIconTextCommandList StartPagePollV2::getLvIconsAndTexts(const int &version)
{
    Q_UNUSED(version);
    return DevicePollCodeSelectorHelper::getLvIconsAndTexts(MATILDA_PROTOCOL_VERSION_V5, DEV_POLL_EMULATOR_L2);
}

//---------------------------------------------------------------------------

OneDevicePollCodes StartPagePollV2::getDeviceSelectSett4adev(const int &devtype)
{
    OneDevicePollCodes codes;

    bool isUnknownDevType = false;
    switch(devtype){
    case UC_METER_ELECTRICITY   : codes = DevicePollCodeSelectorHelper::getElectricityMetersSelectSett(true)  ; break;
    case UC_METER_WATER         : codes = DevicePollCodeSelectorHelper::getWaterMetersSelectSett()        ; break;
    default: isUnknownDevType = true; break;
    }

    if(!isUnknownDevType)
        return codes;

    codes.currentPollCode = 0;
    codes.pollcodesetts.insert(0, OnePollCodeSett());
    codes.pollcodesnames.append(PollCodeName2data("name", 0, "iconstr"));
    //    codes.datetimesett = SelectDateTimeSett() is is already ready for using

    return codes;
}

//---------------------------------------------------------------------------

QVariant StartPagePollV2::getPageSett4read(bool &ok, QString &mess)
{
    emit onReloadAllMeters();

    Q_UNUSED(ok);
    Q_UNUSED(mess);

    QTimer::singleShot(1, this, SLOT(onPbReadClicked()) );
    return QVariant();

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

void StartPagePollV2::onUpdatedSavedList(int activeMetersSize, int switchedOffMetersSize, int meterElectricityActive, int metersWaterActive)
{
    Q_UNUSED(activeMetersSize);
    Q_UNUSED(switchedOffMetersSize);
    emit setLblWaitTxtDatabase(tr("Saved list (electricity - %1, water - %2)").arg(meterElectricityActive).arg(metersWaterActive));
}

//---------------------------------------------------------------------------

void StartPagePollV2::getMetersLater()
{
    QTimer::singleShot(333, this, SIGNAL(onReloadAllMeters()));
}

//-----------------------------------------------------------------------

void StartPagePollV2::onPbReadClicked()
{

    gHelper->updateSettDateMaskAndDotPos();


    const StartPollTabSettExt sett = lastSelsett = getPollCodeSett4read();

    QString mess;
    metersListMedium->setLastPageId(accessibleName());

    //           hash.insert("ni", lni.join(","));//  ui->leMeterDataOnlyThisNI->text().trimmed());

    if(!sett.ok){
        gHelper->showMess(sett.txt);
        return;
    }

    if(!sett.hashSelSett.value("ni").toString().split(",", QString::SkipEmptyParts).isEmpty()){
        //start poll directly

        if(startPollOneMeterMode(lastSelsett, mess)){
            //create tab
        }else{
            gHelper->showMess(mess);
        }
        return;
    }

    if(startPollAllMetersMode(lastSelsett, mess)){

    }else{
        gHelper->showMess(mess);
    }

}

//---------------------------------------------------------------------------

void StartPagePollV2::setWaterSleepSett(bool sendSleepCommand, int secs, bool checkProfile)
{
    lastWtrSett.isValid = true;
    lastWtrSett.sendSleepCommand = sendSleepCommand;
    lastWtrSett.secs = secs;
    lastWtrSett.checkProfile = checkProfile;
    //poll
}

//---------------------------------------------------------------------------

void StartPagePollV2::setNi2model(QStringList modelnis)
{
    lTempPollSett.lastLineModelNiPasswd = modelnis.join(" ");
}

//---------------------------------------------------------------------------

void StartPagePollV2::command4devSlot(quint16 command, QVariantMap map)
{
    createTab(lastSelsett);
    emit command4dev(command, map);
    //    SettLoader::saveSett(SETT_ZBRTR_POLLPAGE_MODE, ui->tbwTypeOfMeter->currentIndex());
}

void StartPagePollV2::onSelectMeters4pollKickedOff()
{
    lTempPollSett.isSelectMeters4pollProcessing = false;
    emit lockPbRead(lTempPollSett.isSelectMeters4pollProcessing);

}

void StartPagePollV2::onCurrentProcessingTabKilledSlot()
{
    if(lTempPollSett.lastWdgtActive.isEmpty())
        return;

    emit killCurrentTask();


}
//---------------------------------------------------------------------------
void StartPagePollV2::onLockButtons(bool disable)
{
    if(!disable)
        lTempPollSett.lastWdgtActive.clear();
}

//---------------------------------------------------------------------------

bool StartPagePollV2::startPollOneMeterMode(const StartPollTabSettExt &selsett, QString &mess)
{
    QDateTime dtFrom, dtTo;
    lTempPollSett.lastLineModelNiPasswd.clear();


    if(!dtTo.isValid())
        dtTo = QDateTime::currentDateTime();

    const QStringList nis = selsett.hashSelSett.value("ni").toString().split(",", QString::SkipEmptyParts);

    const bool isElectricity = (selsett.meterType == UC_METER_ELECTRICITY);
    //const quint8 &meterType, const QString &line, const QString &cbxText, const QString &passwd, const QString &enrgTxt, const int &tariff, const QDateTime &dtTo, const QDateTime &dtFrom, const bool &ignoreExistingData, const int &go2sleepSecs, const QString &sleepProfileLine, QString &mess
    const QString args = isElectricity ? getArgsElectricMeter(nis, dtTo, dtFrom, mess) :
                                         getArgsWaterMeter(nis, dtTo, dtFrom, mess);

    if(args.isEmpty())
        return false;

    createTab(selsett);

    emit command4dev(selsett.code, args);

    return true;

}

//---------------------------------------------------------------------------

bool StartPagePollV2::startPollAllMetersMode(const StartPollTabSettExt &selsett, QString &mess)
{

    if(lTempPollSett.isSelectMeters4pollProcessing){
        mess = tr("Database is busy. Please wait");
        return false;
    }

    switch(lastSelsett.meterType){

    case UC_METER_ELECTRICITY:{

        break;}

    case UC_METER_WATER      :{

        openWtrSleepDlg();//It must be before SelectMeters4poll
        if(!lastWtrSett.isValid){
            mess = tr("aborted");
            return false;
        }

        break;}

    }
    emit lockPbRead(true);

    lTempPollSett.isSelectMeters4pollProcessing = true;
    GetDataFromDbDoneSignalizator *signalizator = new GetDataFromDbDoneSignalizator(this);
    connect(signalizator, SIGNAL(onDbSelectorKickedOff()), this, SLOT(onSelectMeters4pollKickedOff()));


    SelectMeters4poll *w = new SelectMeters4poll(signalizator, gHelper,  this);
    connect(this, SIGNAL(killSelectMeters4poll()), w, SLOT(deleteLater()) );
    connect(w, SIGNAL(onReloadAllMeters()), this, SIGNAL(onReloadAllMeters()) );
    connect(w, SIGNAL(command4dev(quint16,QVariantMap)), this, SLOT(command4devSlot(quint16,QVariantMap)));
    connect(this, SIGNAL(command4dev(quint16,QString)), w, SLOT(deleteLater()) );
    connect(this, SIGNAL(command4dev(quint16,QVariantMap)), w, SLOT(deleteLater()) );

    connect(metersListMedium, SIGNAL(onAllMeters(UniversalMeterSettList)), w, SIGNAL(onAllMeters(UniversalMeterSettList)) );

    const QDateTime dtFrom = selsett.hashSelSett.value("FromDT").toDateTime();
    const QDateTime dtTo = selsett.hashSelSett.value("ToDT").toDateTime();
    const quint8 pollCode = selsett.code;


    switch(lastSelsett.meterType){

    case UC_METER_ELECTRICITY:{
        w->setPollSettElectric(dtFrom, dtTo, pollCode);
        break;}

    case UC_METER_WATER      :{
        w->setPollSettWater(dtFrom, dtTo, pollCode, lastWtrSett.sendSleepCommand, lastWtrSett.secs, lastWtrSett.checkProfile);
        break;}

    default: qDebug() << "can't set pollSett StartPagePoll lastSelsett.meterType=" << lastSelsett.meterType;
    }
//    w->setPollSett(dtFrom, dtTo, pollCode, lastSelsett.meterType, (lastSelsett.meterType == UC_METER_WATER && ui->cbxPwrManagement->isChecked()) ? ui->sbSleepSecsAfter->value() : 0);

    emit addWdgt2stackWdgt(w, WDGT_TYPE_ZBYR_SELECT_METERS4POLL, true, tr("Select"), ":/katynko/svg/dialog-ok-apply.svg");
    return true;
}

//---------------------------------------------------------------------------

void StartPagePollV2::createTab(const StartPollTabSettExt &sett)
{
    gHelper->updateSettDateMaskAndDotPos();
    gSett4all->updateTableSett();


    QVariantHash hash = sett.hashSelSett;

    if(true){

        gHelper->updateSettDateMaskAndDotPos();
        gSett4all->updateTableSett();

        QString txt = sett.txt;

        const bool isMeterEvent = (sett.code >= POLL_CODE_READ_METER_LOGBOOK && sett.code <= POLL_CODE_WTR_METER_LOGBOOK);

        DbDataForm *f = new DbDataForm(gHelper,this);
        connect(gHelper, SIGNAL(setPbReadEnableDisable(bool)), f, SIGNAL(checkBlncStateEnNow(bool)) );

        f->setSelectSett(hash.value("FromDT").toDateTime(), hash.value("ToDT").toDateTime(), hash.value("ToDT").toDateTime().isValid(), txt, sett.code);

        f->setAccessibleName(QString::number(QDateTime::currentMSecsSinceEpoch()));
        lastWdgtAccssbltName = f->accessibleName();

        connect(f, SIGNAL(disconnectMeFromAppendData()), this, SLOT(disconnectMeFromAppendData()) );
        if(isMeterEvent)
            connect(this, SIGNAL(appendData2model(QString,QVariantHash)), f, SIGNAL(appendEvData2model(QString,QVariantHash)) );
        else
            connect(this, SIGNAL(appendData2model(QString,QVariantHash)), f, SIGNAL(appendData2model(QString,QVariantHash)) );
        connect(this, SIGNAL(killTabByName(QString))                , f, SLOT(killTabByName(QString))                 );



        txt.append(", ");
        QDateTime dtFrom = hash.value("FromDT").toDateTime().toLocalTime();
        QDateTime toDt = hash.value("ToDT").toDateTime().toLocalTime();
        if(!sett.allowDate2utc){// code == POLL_CODE_READ_END_DAY || code == POLL_CODE_READ_END_MONTH){
            dtFrom = dtFrom.toUTC();
            toDt = toDt.toUTC();
        }


        txt.append(tr("From: %1").arg(dtFrom.toString("yyyy-MM-dd hh:mm:ss")));
        if(hash.value("ToDT").toDateTime().isValid())
            txt.append(", " + tr("To: %1").arg(toDt.toString("yyyy-MM-dd hh:mm:ss")));
        else
            txt.append(", " + tr("To: *"));

        QVariantHash somehash;
        const int lastDbFilterMode = ViewDataHelper::getLastDbModeFromPollCode(sett.code, true, false, somehash) ;//ViewDataHelper::getLastDbModeFromPollCode(sett.code, false);

        f->setPageMode(lastDbFilterMode, txt, sett.allowDate2utc);//update child accebl name


        addThisWidget2tab(f, txt.mid(txt.indexOf(">") + 1), sett.icon);

        lTempPollSett.lastWdgtActive = lastWdgtAccssbltName;
        emit onPollStarted(sett.code, getEnrgList4code(sett.code), gHelper->dateMask, isMeterEvent ? 0 : gHelper->dotPos, sett.allowDate2utc);

    }
    QTimer::singleShot(1, this, SIGNAL(killSelectMeters4poll()));

}

void StartPagePollV2::openWtrSleepDlg()
{
    WaterStartPollSleepDlg *d = new WaterStartPollSleepDlg(lastWtrSett, this);
    connect(d, &WaterStartPollSleepDlg::setWaterSleepSett, this, &StartPagePollV2::setWaterSleepSett);
    lastWtrSett.isValid = false;
    d->exec();
    d->deleteLater();

}

//---------------------------------------------------------------------------
