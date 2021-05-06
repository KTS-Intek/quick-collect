#include "startpagepollv2.h"


///[!] quick-collect
#include "zbyrator-src/selectmeters4poll.h"


///[!] guisett-shared-core
//#include "src/nongui/settloader.h"

///[!] dataprocessing
#include "src/matilda/devicepollcodeselectorhelper.h"
#include "src/matilda/viewdatahelper.h"


///[!] widgets-shared
#include "dataconcentrator-pgs/dbdataform.h"


///[!] widgets-meters
#include "dataconcetrator-pgs/templates/quickcollectstartpollparamswdgt.h"


///[!] zbyrator-shared
#include "src/meter/meterstatehelper.h"



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

MyPollCodeList StartPagePollV2::getLvIconsAndTexts(const int &version)
{
    Q_UNUSED(version);
    return DevicePollCodeSelectorHelper::getLvIconsAndTexts(MATILDA_PROTOCOL_VERSION_V5, DEV_POLL_EMULATOR_L2);
}



//---------------------------------------------------------------------------


void StartPagePollV2::createTab(const StartPollTabSettExt &sett)
{


    DbDataForm *dbData = createTemplateDataForm(sett);
    lTempPollSett.lastWdgtActive = lastWdgtAccssbltName;
    emit onPollStarted(sett.select.pollCode, getEnrgList4code(sett.select.pollCode), gHelper->dateMask, gHelper->dotPos, true);// sett.allowDate2utc);


    //you must create some class that can process data from zbyrator to GUI


    QTimer::singleShot(1, this, SIGNAL(killSelectMeters4poll()));


}

//---------------------------------------------------------------------------

bool StartPagePollV2::canContinueWithTheseSettings(const StartPollTabSettExt &sett)
{
    Q_UNUSED(sett);
    metersListMedium->setLastPageId(accessibleName());

    return true;
}

//---------------------------------------------------------------------------

bool StartPagePollV2::createObjectsForPollAllMetersMode(const StartPollTabSettExt &selsett, QString &mess)
{
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

    const QDateTime dtFrom = QDateTime::fromMSecsSinceEpoch(selsett.select.timeRange.msecFrom);// selsett.hashSelSett.value("FromDT").toDateTime();
    const QDateTime dtTo = QDateTime::fromMSecsSinceEpoch(selsett.select.timeRange.msecTo);// selsett.hashSelSett.value("ToDT").toDateTime();
    const quint8 pollCode = selsett.select.pollCode;


    switch(lastSelsett.deviceType){

    case UC_METER_ELECTRICITY:{
//        w->setPollSettElectric(dtFrom, dtTo, pollCode, getIgnoreRetries());
        break;}

    case UC_METER_WATER      :{
//        w->setPollSettWater(dtFrom, dtTo, pollCode, lastWtrSett.sendSleepCommand, lastWtrSett.secs, lastWtrSett.checkProfile, getIgnoreRetries());
        break;}

    default: qDebug() << "can't set pollSett StartPagePoll lastSelsett.deviceType=" << lastSelsett.deviceType;
        mess = tr("Couldn't start the poll. The unknown device type '%1'").arg(int(lastSelsett.deviceType)); return false;
    }
    emit addWdgt2stackWdgt(w, WDGT_TYPE_ZBYR_SELECT_METERS4POLL, true, tr("Select"), ":/katynko/svg/dialog-ok-apply.svg");

    return true;
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

//---------------------------------------------------------------------------

void StartPagePollV2::addQuickPollPanel()
{
    startTab->disconnectLblWaitTxtDataBase();

    QuickCollectStartPollParamsWdgt *w = new QuickCollectStartPollParamsWdgt(this);
    startTab->addTopWidget(w);

    connect(startTab, &SelectFromDatabaseTemplateStartTab::setLblWaitTxtDatabase, w, &QuickCollectStartPollParamsWdgt::setLblWaitTxtDatabase);
    connect(w, &QuickCollectStartPollParamsWdgt::onCbxIgnoreRetr, this ,&StartPagePollV2::onCbxIgnoreRetr);
    connect(w, &QuickCollectStartPollParamsWdgt::onCbxOnlyGlobalConnection, this, &StartPagePollV2::onCbxOnlyGlobalConnection);

    connect(this, &StartPagePollV2::requestToSwitchIgnoreCycles, w, &QuickCollectStartPollParamsWdgt::requestToSwitchIgnoreCycles);
    QTimer::singleShot(3333, w, SLOT(sendYourCbxStates()));

}

//---------------------------------------------------------------------------
