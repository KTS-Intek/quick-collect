#include "qcmainwindow.h"
#include "ui_qcmainwindow.h"

///[!] quick-collect
#include "zbyrator-src/ktsconnectwdgt.h"
#include "zbyrator-src/startexchange.h"
#include "zbyrator-src/zbyratoroptions.h"
#include "zbyrator-src/createtoolbar4quickcollect.h"


///[!] quick-collect-gui-core
#include "quick-collect-gui-core/matilda-bbb-emulator/matildabbbcover.h"
#include "quick-collect-gui-core/processes/zbyratorprocessmanager.h" //launches SQLiteMediumLocalServer


///[!] guisett-shared-core
#include "src/nongui/settloader.h"
#include "src/nongui/oneinstancewatcher.h"

///[!] guisett-shared
#include "guisett-shared-src/appversion.h"


///[!] widgets-shared
#include "main-pgs/langdialog.h"
#include "gui-src/stackwidgethelper.h"
#include "template-pgs/smplptewdgt.h"
#include "gui-src/guihelperdefines.h"
#include "main-pgs/custommessagewidget.h"
#include "main-pgs/scanipwidget.h"
#include "gui-src/actionmaintanenance.h"
#include "main-pgs/appproxysett.h"


///[!] widgets-meters
#include "dataconcetrator-pgs/emeterlistwdgt.h"
#include "dataconcetrator-pgs/wmeterlistwdgt.h"
#include "dataconcetrator-pgs/pmeterlistwdgt.h"

#include "dataconcetrator-pgs/src/databasewdgt4quickcollect.h"
#include "dataconcetrator-pgs/editenergywidget.h"




///[!] zbyrator-base
#include "src/zbyrator-v2/metermanager.h"


#include <QDebug>
#include <QTimer>
#include <QLocale>
#include <QDesktopWidget>
#include <QStyleFactory>

#include "moji_defy.h"


//#include "src/matilda/classmanagerudpscanner.h"



QcMainWindow::QcMainWindow(const QFont &font4log, const int &defFontPointSize, const bool &verboseMode, QWidget *parent) :
    MainIfaceMedium(font4log, defFontPointSize, verboseMode, parent),
    ui(new Ui::QcMainWindow)
{
    ui->setupUi(this);



}

//---------------------------------------------------------------------

QcMainWindow::~QcMainWindow()
{
    delete ui;
}

void QcMainWindow::initPage()
{
    lastToolBar = ui->mainToolBar;
    ui->statusBar->hide();
    createGuiHelper(ui->stackedWidget, guiSett->verboseMode);// 12, MATILDA_PROTOCOL_VERSION);
    createStackWidgetHelper();
    createAppOutLog();

    connect(ui->stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(onStackedWidgetCurrentChanged(int)) );



    createToolBar();
    createMeterListManager();
    createOneInstanceChecker();

    connect(this, SIGNAL(onRequest2pollThese(QStringList,quint8)), this, SLOT(activatePageHome()));
    connect(this, SIGNAL(onRequest2GetDataThese(QStringList,quint8)), this, SLOT(activatePageDb()));


    QTimer::singleShot(999, this, SLOT(activatePageHome()));

}

void QcMainWindow::retranslateWidgets()
{
    ui->retranslateUi(this);
    qDebug() << "retranslateWidgets";
}



//---------------------------------------------------------------------

void QcMainWindow::onActivateThisWdgt(QString tabData, bool andShowIt)
{
    //stackContainsThisWdgt(QStackedWidget *stackedWidget, const QString &wdgtName, const bool andChange, const int indxFrom)
    if(!WidgetsHelper::stackContainsThisWdgt(ui->stackedWidget, tabData, andShowIt))
        createOneOfMainWdgt(tabData, andShowIt);

    if(andShowIt)
        emit addWdgt2history();
}

void QcMainWindow::continueCreatingObjects()
{
    connect(this, &QcMainWindow::receivedKillSignal, this, &QcMainWindow::onAppIsKilling);
    currWidgetSett.allow2close = false;
    createZbyrProcManager();
    createMeterManager();
    createMatildaBBBcover();

    connect(ui->actionExit, SIGNAL(triggered(bool)), this, SLOT(close()));

    ui->menuHelp->insertAction(ui->actionAbout, new ActionMaintanenance(guiHelper->verboseMode, this));
    setEnabled(true);

    checkProxySettLater();
    QTimer::singleShot(33333, this, SLOT(createUpdateChecker()));


}
//---------------------------------------------------------------------
void QcMainWindow::onAppIsKilling()
{
    saveAllSettBeforeExitIfWidgetWasReady();
    setEnabled(false);
    hide();
    qDebug() << "onAppIsKilling ";
#ifdef Q_OS_LINUX
    QTimer::singleShot(1111, this, SLOT(allow2closeTheWindowAndClose()));
#else
    QTimer::singleShot(2555, this, SLOT(allow2closeTheWindowAndClose()));
#endif
}

//---------------------------------------------------------------------

void QcMainWindow::checkDbPageIsReady()
{
    onActivateThisWdgt("Database", false);
}

//---------------------------------------------------------------------

void QcMainWindow::activatePageDb()
{
    emit tryToActivateThisPage("Database");

}

//---------------------------------------------------------------------

void QcMainWindow::activatePageHome()
{
    emit tryToActivateThisPage("Exchange");

}

void QcMainWindow::checkProxySett()
{
    if(!guiHelper->isProxyOk()){
        showMessage(tr("You must setup the proxy"));
        return;
    }
}



//---------------------------------------------------------------------

void QcMainWindow::createOneOfMainWdgt(const QString &tabData, const bool &andActivateIt)
{
    qDebug() << "create " << tabData;

    GuiHelper *gHelper = guiHelper;

    const int row = CreateToolBar4quickCollect::getTabsData().indexOf(tabData);
    MatildaConfWidget *w = 0;
    int readCommand = 1, writeCommand = 0xFFFE;

    switch(row){
    case 0: w = createStartExchangeWdgt(gHelper,  this);  break;
    case 1: w = createElectricityMeterListWdgt(    gHelper,  this); break;
    case 2: w = createWaterMeterListWdgt(    gHelper,  this); break;
    case 3: w = createPulseMeterListWdgt(    gHelper,  this); break;

    case 4: w = createDatabasePage(       gHelper,  this) ; readCommand = COMMAND_READ_DATABASE; break;  //    l.append( QString("Database") );
//    case 4: w = createMeterLogsPage(   gHelper,  this) ; break; //    l.append( QString("Meter logs") );
//    case 5: w = new KtsConnectWdgt(     gHelper,  this); break;
    case 5: w = createPageLog(gHelper,  this)  ; break;   //    l.append( QString("State")                   );

    }

    if(w){
        try{
            w->objectName();
        }catch(...){
            w = 0;
            return;
        }
    }

    if(w){
        const TabName2icosPaths ttn = CreateToolBar4quickCollect::getTabs4quickCollect().at(row);

        w->setWindowTitle(ttn.tabName);
        w->setWindowIcon(QIcon(ttn.path));


//        connect(this, SIGNAL(killMyChild()), w, SLOT(deleteLater()) );
//        connect(this, SIGNAL(onOperationNError(int)), w, SLOT(onOperationNError(int)) );
//        w->setRwCommand(MatildaDeviceTree::getPageCanRead().at(row), MatildaDeviceTree::getPageCanWrite().at(row));

//        w->setRwCommand(readCommand, writeCommand);
//        w->setHasDataFromRemoteDevice();
        w->setupGlobalLblMessage(ui->lblPageMess);
        QWidget *sa = StackWidgetHelper::addWdgtWithScrollArea(this, w, tabData);
        ui->stackedWidget->addWidget(sa);
        if(andActivateIt)
            ui->stackedWidget->setCurrentWidget(sa);
    }
}

//---------------------------------------------------------------------

void QcMainWindow::createToolBar()
{
    CreateToolBar4quickCollect *c = new CreateToolBar4quickCollect(this);
    connect(c, SIGNAL(onActivateThisWdgt(QString,bool)), this, SLOT(onActivateThisWdgt(QString,bool)) );
    connect(this, &QcMainWindow::tryToActivateThisPage, c, &CreateToolBar4quickCollect::tryToActivateThisPage);
    c->createToolBarItems(ui->mainToolBar);
}
//---------------------------------------------------------------------
void QcMainWindow::createAppOutLog()
{
     setupAppLogAction(ui->actionThe_log_of_the_application, ui->widget4log);

}

//---------------------------------------------------------------------

void QcMainWindow::createZbyrProcManager()
{
    //It is for the sqlite-medium module
    ZbyratorProcessManager *m = new ZbyratorProcessManager;
    QThread *t = new QThread;
t->setObjectName("ZbyratorProcessManager");
    m->moveToThread(t);

//    connect(this, &QcMainWindow::destroyed, m, &ZbyratorProcessManager::deleteLater);
    connect(this, &QcMainWindow::receivedKillSignal, m, &ZbyratorProcessManager::kickOffAllObjects);

    connect(m, SIGNAL(destroyed(QObject*)), t, SLOT(quit()) );
    connect(t, &QThread::finished, t, &QThread::deleteLater);

    connect(t, SIGNAL(started()), m, SLOT(onThreadStarted()) );

    QTimer::singleShot(1111, t, SLOT(start()));


}

//---------------------------------------------------------------------

void QcMainWindow::createMeterManager()
{
//it is a part of zbyrator-bbb
    MeterManager *zbyrator = new MeterManager(QString(), guiSett->verboseMode, ZbyrConnSett());

    QThread *thread = new QThread;
    thread->setObjectName("MeterManager");


    zbyrator->moveToThread(thread);

    connect(this, &QcMainWindow::receivedKillSignal, zbyrator, &MeterManager::kickOffAllObjects);
    connect(zbyrator, SIGNAL(destroyed(QObject*)), thread, SLOT(quit()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));


    connect(thread, SIGNAL(started()), zbyrator, SLOT(onThreadStarted()));

//    connect(extSocket, &ZbyratorSocket::appendDbgExtData, this, &ZbyratorManager::appendDbgExtData );



//    connect(zbyrator, &MeterManager::command2extensionClient, extSocket, &ZbyratorSocket::command2extensionClient   );
//    connect(zbyrator, &MeterManager::onAboutZigBee          , extSocket, &ZbyratorSocket::sendAboutZigBeeModem      );


    connect(metersListMedium, SIGNAL(command4dev(quint16,QString))    , zbyrator, SIGNAL(command4devStr(quint16,QString)) );
    connect(metersListMedium, SIGNAL(command4dev(quint16,QVariantMap)), zbyrator, SIGNAL(command4dev(quint16,QVariantMap)) );



    connect(metersListMedium, &ZbyrMeterListMedium::onReloadAllMeters2zbyrator, zbyrator, &MeterManager::onReloadAllMetersIgnoreOff    );
    connect(metersListMedium, &ZbyrMeterListMedium::onConfigChanged     , zbyrator, &MeterManager::onConfigChanged      );
    connect(metersListMedium, &ZbyrMeterListMedium::sendMeAlistOfMeters , zbyrator, &MeterManager::sendMeAlistOfMeters  );

    connect(metersListMedium, &ZbyrMeterListMedium::setThisIfaceSett    , zbyrator, &MeterManager::setThisIfaceSett     );
//    connect(metersListMedium, &ZbyrMeterListMedium::setPollSaveSettings , zbyrator, &MeterManager::setPollSaveSettings  ); zbyrator has it's own
    connect(metersListMedium, SIGNAL(setPollSaveSettings(quint16,quint16,bool,bool,bool,qint32,qint32,qint32,bool)), zbyrator, SLOT(reloadPollSettings()));
    connect(metersListMedium, &ZbyrMeterListMedium::reloadIfaceChannels, zbyrator, &MeterManager::reloadIfaceChannels); // SIGNAL(setPollSaveSettings(quint16,quint16,bool,bool,bool,qint32,qint32,qint32,bool)), zbyrator, SLOT(reloadIfaceChannels()));

    connect(metersListMedium, &ZbyrMeterListMedium::giveMeYourCache     , zbyrator, &MeterManager::giveMeYourCache      );
    connect(metersListMedium, &ZbyrMeterListMedium::killUconTasks       , zbyrator, &MeterManager::killUconsTasks       );
    connect(metersListMedium, &ZbyrMeterListMedium::setIgnoreCycles     , zbyrator, &MeterManager::setIgnoreCycles      );
    connect(metersListMedium, &ZbyrMeterListMedium::setOnlyGlobalConnection     , zbyrator, &MeterManager::setOnlyGlobalConnection      );

    connect(metersListMedium, &ZbyrMeterListMedium::reloadSavedSleepProfiles, zbyrator, &MeterManager::reloadSavedSleepProfiles      );
    connect(metersListMedium, &ZbyrMeterListMedium::reloadSettings, zbyrator, &MeterManager::reloadSettings);

    connect(metersListMedium, SIGNAL(showMessage(QString)), this, SLOT(showMessage(QString)) );

    connect(zbyrator, SIGNAL(checkThisMeterInfo(UniversalMeterSett)), metersListMedium, SIGNAL(onReloadAllMeters2zbyrator()) );

    connect(zbyrator, &MeterManager::onAllMeters                , metersListMedium, &ZbyrMeterListMedium::onAllMetersSlot           );
    connect(zbyrator, &MeterManager::onAllMeters                , metersListMedium, &ZbyrMeterListMedium::onAllMeters               );
    connect(zbyrator, &MeterManager::onAlistOfMeters            , metersListMedium, &ZbyrMeterListMedium::onAlistOfMeters           );
    connect(zbyrator, &MeterManager::setVirtualMetersSett       , metersListMedium, &ZbyrMeterListMedium::setVirtualMetersSett      );
    connect(zbyrator, &MeterManager::ifaceLogStr                , metersListMedium, &ZbyrMeterListMedium::ifaceLogStr               );
    connect(zbyrator, &MeterManager::ifaceLogStrNonBuf    , metersListMedium, &ZbyrMeterListMedium::ifaceLogStrFromZbyrator               );

    //    connect(metersListMedium, &ZbyrMeterListMedium::ifaceLogStr, w, &SmplPteWdgt::appendPteText);


    connect(zbyrator, &MeterManager::appendMeterData            , metersListMedium, &ZbyrMeterListMedium::appendMeterData           );
    connect(zbyrator, &MeterManager::onConnectionStateChanged   , metersListMedium, &ZbyrMeterListMedium::onConnectionStateChanged  );
    connect(zbyrator, &MeterManager::onUconStartPoll            , metersListMedium, &ZbyrMeterListMedium::onUconStartPoll           );
    connect(zbyrator, &MeterManager::onReadWriteOperation       , metersListMedium, &ZbyrMeterListMedium::onReadWriteOperation      );

//    connect(zbyrator, &MeterManager::onStatisticChanged         , metersListMedium, &ZbyrMeterListMedium::onStatisticChanged        ); do not use it, everything must work without it
    connect(zbyrator, &MeterManager::onAllStatHash              , metersListMedium, &ZbyrMeterListMedium::onAllStatHash             );
    connect(zbyrator, &MeterManager::onTaskTableChanged         , metersListMedium, &ZbyrMeterListMedium::onTaskTableChanged        );
    connect(zbyrator, &MeterManager::onTaskCanceled             , metersListMedium, &ZbyrMeterListMedium::onTaskCanceled            );
    connect(zbyrator, &MeterManager::meterRelayStatus           , metersListMedium, &ZbyrMeterListMedium::meterRelayStatus          );

    connect(zbyrator, &MeterManager::meterDateTimeDstStatus     , metersListMedium, &ZbyrMeterListMedium::meterDateTimeDstStatus    );
    connect(zbyrator, &MeterManager::waterMeterSchedulerStts    , metersListMedium, &ZbyrMeterListMedium::waterMeterSchedulerStts   );

    connect(zbyrator, &MeterManager::appendAppOut, this, &QcMainWindow::appendShowMessagePlain);


    connect(zbyrator, &MeterManager::command2extensionClient, metersListMedium, &ZbyrMeterListMedium::command2extensionClient   );
    connect(zbyrator, &MeterManager::onAboutZigBee          , metersListMedium, &ZbyrMeterListMedium::onAboutZigBee      );
    connect(zbyrator, &MeterManager::relayStatusChanged     , metersListMedium, &ZbyrMeterListMedium::relayStatusChanged        );


    connect(zbyrator, &MeterManager::onConnectionStateChanged, guiHelper, &GuiHelper::setPbWriteEnableDisableSlot);// ReadEnableDisableSlot);

    connect(zbyrator, &MeterManager::requestToSwitchIgnoreCycles, metersListMedium, &ZbyrMeterListMedium::requestToSwitchIgnoreCycles);


    connect(zbyrator, SIGNAL(onConnectionStateChanged(bool)), metersListMedium, SIGNAL(onTaskTableChanged()));

//    connect(guiHelper, SIGNAL(mWrite2RemoteDev(quint16,QVariant,QWidget*)), metersListMedium, SLOT(mWrite2RemoteDev(quint16,QVariant)));

    metersListMedium->importGroups2metersFile();
    QTimer::singleShot(1111, thread, SLOT(start()) );

}

void QcMainWindow::createMeterListManager()
{
    //it is a joint point between GUI and zbyrator-bbb
    metersListMedium = new ZbyrMeterListMedium(guiHelper->verboseMode, this);

    guiHelper->ucDeviceTreeW = metersListMedium->ucDeviceTreeW;

    metersListMedium->activateEmul2DeviceType();

    //    systeminfo.appv

    connect(this, &QcMainWindow::receivedKillSignal, metersListMedium, &ZbyrMeterListMedium::killAllObjects);

    connect(metersListMedium, &ZbyrMeterListMedium::setPbReadEnableDisable, guiHelper, &GuiHelper::setPbReadEnableDisableSlot);
//    connect(metersListMedium, &ZbyrMeterListMedium::updateHashSn2meter, guiHelper, &GuiHelper::updateHashSn2meter);
//    connect(metersListMedium, &ZbyrMeterListMedium::data2dbMedium      , guiHelper, &GuiHelper::updateSettDateMaskAndDotPos);

//    connect(metersListMedium, &ZbyrMeterListMedium::setElectricityPowerCenters, guiHelper, &GuiHelper::setElectricityPowerCenters);
//    connect(metersListMedium, &ZbyrMeterListMedium::setWaterPowerCenters, guiHelper, &GuiHelper::setWaterPowerCenters);
    connect(metersListMedium, &ZbyrMeterListMedium::pbStopAnimateClick, guiHelper, &GuiHelper::pbStopAnimateClick);
    guiHelper->setObjectName("QcMainWindow");

    connect(metersListMedium, &ZbyrMeterListMedium::appendAppLog        , this, &QcMainWindow::appendShowMessagePlain );

    guiHelper->managerEnDisBttn.pbReadDis = guiHelper->managerEnDisBttn.pbWriteDis = false;
    connect(this, &QcMainWindow::reloadSettings2ucEmulator, metersListMedium, &ZbyrMeterListMedium::reloadSettings);
    connect(guiHelper, &GuiHelper::setPbWriteEnableDisable, metersListMedium, &ZbyrMeterListMedium::setPbWriteDis);
    metersListMedium->setPbWriteDis(guiHelper->managerEnDisBttn.pbWriteDis);




    connect(metersListMedium, SIGNAL(setPollSaveSettings(quint16,quint16,bool,bool,bool,qint32,qint32,qint32,bool)), metersListMedium, SIGNAL(reloadIfaceChannels()));

}

//---------------------------------------------------------------------

void QcMainWindow::createMatildaBBBcover()
{
    const QString nameStr = "Matilda";
    const bool verbose = guiSett->verboseMode;

    MatildaBBBcover *c = new MatildaBBBcover(nameStr, verbose);
    QThread *t = new QThread;
    t->setObjectName("MatildaBBBcover");
    c->moveToThread(t);

//    connect(this, SIGNAL(destroyed(QObject*)), c, SLOT(deleteLater()));
    connect(this, &QcMainWindow::receivedKillSignal, c, &MatildaBBBcover::itIsTime2kickOffWithDelay);

    connect(this, &QcMainWindow::command2extensionBBB, c, &MatildaBBBcover::command2extension);
    connect(c, SIGNAL(destroyed(QObject*)), t, SLOT(quit()));
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));

    connect(t, SIGNAL(started()), c, SLOT(onThreadStarted()));

    connect(this, &QcMainWindow::reloadSettings2ucEmulator, c, &MatildaBBBcover::reloadSettings2ucEmulator);
    QTimer::singleShot(4444, t, SLOT(start()));// t->start();

//    t->start();
}

void QcMainWindow::createOneInstanceChecker()
{
    setEnabled(false);
    OneInstanceWatcher *watcher = new OneInstanceWatcher("ktsintek.com.ua.zbyrator", "kts-intek.com.ua", guiHelper->verboseMode);
    QThread *t = new QThread;
    t->setObjectName("OneInstanceWatcher");
    watcher->moveToThread(t);

    connect(this, &QcMainWindow::receivedKillSignal, watcher, &OneInstanceWatcher::kickOffAllObjects);
    connect(watcher, SIGNAL(destroyed(QObject*)), t, SLOT(quit()));
    connect(t, &QThread::finished, t, &QThread::deleteLater);

    connect(t, SIGNAL(started()), watcher, SLOT(onThrdStarted()));

    connect(watcher, &OneInstanceWatcher::iAmAlone, this, &QcMainWindow::continueCreatingObjects);
    connect(watcher, &OneInstanceWatcher::onNeed2setFocus, this, &QcMainWindow::onNeed2setFocus);
    connect(watcher, &OneInstanceWatcher::iAmNotAlone, this, &QcMainWindow::killThisInstanceOfTheAppWithoutSaving);
    t->start();

}

//---------------------------------------------------------------------

MatildaConfWidget *QcMainWindow::createStartExchangeWdgt(GuiHelper *gHelper, QWidget *parent)
{
//    GuiHelper *guiHelper = new GuiHelper(this);
//    guiHelper->initObj();
//    guiHelper->guiSett = guiSett;
//    guiHelper->lDevInfo = gHelper->lDevInfo;

//    guiHelper->cacheHelper = gHelper->cacheHelper;
//    guiHelper->stackedWidget = gHelper->stackedWidget;
//    guiHelper->parentWidget = this;


    StartExchange *w = new StartExchange(gHelper,  parent);
    w->metersListMedium = metersListMedium;
    this->lastIfaceLoader4mainwindow = metersListMedium->ifaceLoaderPrimary;

    connect(w, SIGNAL(openEditMacProfileWdgt(bool,QLineEdit*)), this, SLOT(openEditMacProfileWdgt(bool,QLineEdit*)));

    connect(this, &QcMainWindow::appendShowMessage, w, &StartExchange::appendShowMessagePlain);
    connect(this, &QcMainWindow::appendShowMessagePlain, w, &StartExchange::appendShowMessagePlain);

    connect(this, &QcMainWindow::onRequest2pollThese, w, &StartExchange::onRequest2pollThese);


    connect(w, SIGNAL(pageReady()), this, SIGNAL(initDone()));
    connect(w, &StartExchange::onRequest2GetDataThese, this, &QcMainWindow::onRequest2GetDataThese);

    connect(this, &QcMainWindow::receivedKillSignal, w, &StartExchange::receivedKillSignal);

    return w;
}

//---------------------------------------------------------------------

MatildaConfWidget *QcMainWindow::createElectricityMeterListWdgt(GuiHelper *gHelper, QWidget *parent)
{

    EMeterListWdgt *w = new EMeterListWdgt(gHelper,  parent);

    connect(w, &EMeterListWdgt::onUserChangedTheModel, [=]{
        QString message;
        const QString result = w->pushPageContent(message);
        if(!result.isEmpty() && result != "emeter"){
            //it must be updated only once
            w->setAccessibleName("emeter");
            disconnect(gHelper->ucDeviceTreeW, &UCDeviceTreeWatcher::onUCEMeterSettingsChanged, w, &EMeterListWdgt::onUCEMeterSettingsChanged);
        }
    });


    connect(w, &EMeterListWdgt::onRequest2pollThese, this, &QcMainWindow::onRequest2pollThese);
    connect(w, &EMeterListWdgt::onRequest2GetDataThese, this, &QcMainWindow::onRequest2GetDataThese);
    connect(w, &EMeterListWdgt::checkDbPageIsReady, this, &QcMainWindow::checkDbPageIsReady);

    connect(w, SIGNAL(onRequest2pollThese(QStringList,quint8)), this, SLOT(activatePageHome()));
    connect(w, SIGNAL(onRequest2GetDataThese(QStringList,quint8)), this, SLOT(activatePageDb()));

    return w;


}
//---------------------------------------------------------------------

MatildaConfWidget *QcMainWindow::createWaterMeterListWdgt(GuiHelper *gHelper, QWidget *parent)
{

    WMeterListWdgt *w = new WMeterListWdgt(gHelper,  parent);


    connect(w, &WMeterListWdgt::onUserChangedTheModel, [=]{
        QString message;
        const QString result = w->pushPageContent(message);
        if(!result.isEmpty() && result != "wmeter"){
            //it must be updated only once
            w->setAccessibleName("wmeter");
            disconnect(gHelper->ucDeviceTreeW, &UCDeviceTreeWatcher::onUCWMeterSettingsChanged, w, &WMeterListWdgt::onUCWMeterSettingsChanged);
        }
    });


    connect(w, &WMeterListWdgt::onRequest2pollThese, this, &QcMainWindow::onRequest2pollThese);
    connect(w, &WMeterListWdgt::onRequest2GetDataThese, this, &QcMainWindow::onRequest2GetDataThese);
    connect(w, &WMeterListWdgt::checkDbPageIsReady, this, &QcMainWindow::checkDbPageIsReady);

    connect(w, SIGNAL(onRequest2pollThese(QStringList,quint8)), this, SLOT(activatePageHome()));
    connect(w, SIGNAL(onRequest2GetDataThese(QStringList,quint8)), this, SLOT(activatePageDb()));

    return w;

}
//---------------------------------------------------------------------


MatildaConfWidget *QcMainWindow::createPulseMeterListWdgt(GuiHelper *gHelper, QWidget *parent)
{

    PMeterListWdgt *w = new PMeterListWdgt(gHelper,  parent);


    connect(w, &PMeterListWdgt::onUserChangedTheModel, [=]{
        QString message;
        const QString result = w->pushPageContent(message);
        if(!result.isEmpty() && result != "pmeter"){
            //it must be updated only once
            w->setAccessibleName("pmeter");
            disconnect(gHelper->ucDeviceTreeW, &UCDeviceTreeWatcher::onUCPMeterSettingsChanged, w, &PMeterListWdgt::onUCPMeterSettingsChanged);
        }
    });


    connect(w, &PMeterListWdgt::onRequest2pollThese, this, &QcMainWindow::onRequest2pollThese);
    connect(w, &PMeterListWdgt::onRequest2GetDataThese, this, &QcMainWindow::onRequest2GetDataThese);
    connect(w, &PMeterListWdgt::checkDbPageIsReady, this, &QcMainWindow::checkDbPageIsReady);

    connect(w, SIGNAL(onRequest2pollThese(QStringList,quint8)), this, SLOT(activatePageHome()));
    connect(w, SIGNAL(onRequest2GetDataThese(QStringList,quint8)), this, SLOT(activatePageDb()));

    return w;
}
//---------------------------------------------------------------------

MatildaConfWidget *QcMainWindow::createPageLog(GuiHelper *gHelper, QWidget *parent)
{
    SmplPteWdgt *w = new SmplPteWdgt(tr("Log") , true, true, gHelper,  false, parent);
    connect(w, &SmplPteWdgt::giveMeYourCache, metersListMedium, &ZbyrMeterListMedium::giveMeYourCache);
    connect(metersListMedium, &ZbyrMeterListMedium::ifaceLogStr, w, &SmplPteWdgt::appendPteText);
    w->showClearButton(true);
    return w;
}
//---------------------------------------------------------------------
MatildaConfWidget *QcMainWindow::createDatabasePage(GuiHelper *gHelper, QWidget *parent)
{
    DatabaseWdgt4QuickCollect *w = new DatabaseWdgt4QuickCollect(gHelper,  parent);


//    connect(w, SIGNAL(data2dbMedium(quint16,QVariant)), metersListMedium, SIGNAL(data2dbMedium(quint16,QVariant)) );// do not uset it


    connect(w, &DatabaseWdgt4QuickCollect::stopDbReading, metersListMedium, &ZbyrMeterListMedium::stopReadDatabase);

    connect(metersListMedium, SIGNAL(setLblWaitTxtDatabase(QString)), w, SIGNAL(setLblWaitTxtDatabase(QString)));
//    connect(metersListMedium, SIGNAL(appendDataDatabase(QVariantHash)), w, SLOT(setPageSett(QVariantHash))); //do not use it

    connect(w, &DatabaseWdgt4QuickCollect::pageEndInit, metersListMedium, &ZbyrMeterListMedium::onReloadAllMeters2zbyrator);// do not use it
//    connect(w, &DatabaseWdgt4QuickCollect::onTry2readDb, metersListMedium, &ZbyrMeterListMedium::onReloadAllMeters2zbyrator);

    connect(this, &QcMainWindow::onRequest2GetDataThese, w, &DatabaseWdgt4QuickCollect::onRequest2pollThese);

    connect(w, &DatabaseWdgt4QuickCollect::onRequest2GetDataTheseFromDb, this, &QcMainWindow::onRequest2GetDataThese);//get date
    connect(w, &DatabaseWdgt4QuickCollect::onRequest2pollTheseFromDb, this, &QcMainWindow::onRequest2pollThese );//start a new poll


    connect(this, &QcMainWindow::receivedKillSignal, w, &DatabaseWdgt4QuickCollect::kickOffAll);

//    connect(w, &DatabaseWdgtV2::onRequest2pollTheseFromDb, [=](QStringList nis, quint8 metertype){
//        qDebug() << "onRequest2pollTheseFromDb QcMainWindow " << metertype << nis ;
//    });
    return w;
}

//obsolete
//MatildaConfWidget *QcMainWindow::createMeterLogsPage(GuiHelper *gHelper, QWidget *parent)
//{
//    MeterJournalForm * w = new MeterJournalForm(gHelper,  parent);
//    connect(w, SIGNAL(data2dbMedium(quint16,QVariant)), metersListMedium, SIGNAL(data2dbMedium(quint16,QVariant)) );
//    connect(metersListMedium, SIGNAL(setLblWaitTxtDatabaseMj(QString)), w, SIGNAL(setLblWaitTxtDatabaseMj(QString)));
//    connect(metersListMedium, SIGNAL(appendDataDatabaseMJ(QVariantHash)), w, SLOT(setPageSett(QVariantHash)));
//    return w;
//}

//---------------------------------------------------------------------

void QcMainWindow::onStackedWidgetCurrentChanged(int arg1)
{
    if(arg1 < 0 || !ui->stackedWidget->currentWidget())
        return;
    const QString wdgtAccessibleName = ui->stackedWidget->currentWidget()->accessibleName();
    if(wdgtAccessibleName != lastWdgtAccessibleName){
        guiHelper->closeYourPopupsSlot();
        lastWdgtAccessibleName = wdgtAccessibleName;
    }
}


//---------------------------------------------------------------------
void QcMainWindow::on_actionOptions_triggered()
{
    if(!WidgetsHelper::stackContainsThisWdgtType(ui->stackedWidget, WDGT_TYPE_OPTIONS, true)){

        ZbyratorOptions *w = new ZbyratorOptions(guiHelper, this);

        connect(w, SIGNAL(destroyed(QObject*)), metersListMedium, SIGNAL(reloadSettings()));

        //it has the same name that matilda-bbb uses, but the usage is different
        connect(w, &ZbyratorOptions::command2extension, metersListMedium, &ZbyrMeterListMedium::command2extension);

        connect(w, &ZbyratorOptions::reloadSettings2ucEmulator, this, &QcMainWindow::reloadSettings2ucEmulator);

        //it has not the same name that matilda-bbb uses, but the usage is the same
        connect(w, &ZbyratorOptions::command2extensionBBB, this, &QcMainWindow::command2extensionBBB);

        addWdgt2stackWdgt(w, WDGT_TYPE_OPTIONS, false, "", "");// tr("Options"), ":/katynko/svg/applications-system.svg");
    }
}

void QcMainWindow::on_actionAbout_triggered()
{
    showMessage(AppVersion::getVersion());
}

void QcMainWindow::on_actionExchange_triggered()
{
    activatePageHome();
}

void QcMainWindow::on_actionProxy_settings_triggered()
{
    AppProxySett *w = new AppProxySett(this);
    connect(w, SIGNAL(onSettSaved()), w, SLOT(deleteLater()));
    connect(w, SIGNAL(destroyed(QObject*)), guiHelper, SLOT(updateProxySett()));
    connect(w, SIGNAL(destroyed(QObject*)), this, SLOT(checkProxySettLater()));

    addWdgt2stackWdgt(w, WDGT_TYPE_APPPROXYSETT, false, tr("Proxy"), ":/katynko/svg2/lc_browseview.svg");

}

void QcMainWindow::checkProxySettLater()
{
    QTimer::singleShot(1111, this, SLOT(checkProxySett()));

}
