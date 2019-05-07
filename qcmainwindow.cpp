#include "qcmainwindow.h"
#include "ui_qcmainwindow.h"

///[!] quick-collect
#include "zbyrator-src/createtoolbar.h"
#include "zbyrator-src/ktsconnectwdgt.h"
#include "zbyrator-src/startexchange.h"
#include "zbyrator-src/zbyratoroptions.h"


///[!] quick-collect-gui-core
#include "quick-collect-gui-core/matilda-bbb-emulator/matildabbbcover.h"
#include "quick-collect-gui-core/processes/zbyratorprocessmanager.h" //launches SQLiteMediumLocalServer


///[!] guisett-shared-core
#include "src/nongui/settloader.h"

///[!] guisett-shared
#include "guisett-shared-src/appversion.h"


///[!] widgets-shared
#include "main-pgs/langdialog.h"
#include "gui-src/stackwidgethelper.h"
#include "template-pgs/smplptewdgt.h"
#include "gui-src/guihelperdefines.h"
#include "main-pgs/custommessagewidget.h"
#include "main-pgs/scanipwidget.h"


///[!] widgets-meters
#include "dataconcetrator-pgs/meterlistwdgt.h"
#include "dataconcetrator-pgs/src/databasewdgtv2.h"
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



QcMainWindow::QcMainWindow(const QFont &font4log, const int &defFontPointSize,  QWidget *parent) :
    MainIfaceMedium(font4log, defFontPointSize, parent),
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
    createGuiHelper(ui->stackedWidget, 12, 5);
    createStackWidgetHelper();
    createAppOutLog();

    connect(ui->stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(onStackedWidgetCurrentChanged(int)) );



    createToolBar();
    createZbyrProcManager();
    createMeterManager();
    createMatildaBBBcover();

}

void QcMainWindow::retranslateWidgets()
{
    ui->retranslateUi(this);
    qDebug() << "retranslateWidgets";
}



//---------------------------------------------------------------------

void QcMainWindow::onActivateThisWdgt(QString tabData)
{
    //stackContainsThisWdgt(QStackedWidget *stackedWidget, const QString &wdgtName, const bool andChange, const int indxFrom)
    if(!WidgetsHelper::stackContainsThisWdgt(ui->stackedWidget, tabData, true))
        createOneOfMainWdgt(tabData);

    emit addWdgt2history();
}



//---------------------------------------------------------------------

void QcMainWindow::createOneOfMainWdgt(const QString &tabData)
{
    qDebug() << "create " << tabData;

    GuiHelper *gHelper = guiHelper;

    const int row = CreateToolBar::getTabsData().indexOf(tabData);
    MatildaConfWidget *w = 0;
    int readCommand = 1, writeCommand = 0xFFFE;

    switch(row){
    case 0: w = createStartExchangeWdgt(gHelper,  this);  break;
    case 1: w = createElectricityMeterListWdgt(    gHelper,  this); break;
    case 2: w = createWaterMeterListWdgt(    gHelper,  this); break;

    case 3: w = createDatabasePage(       gHelper,  this) ; readCommand = COMMAND_READ_DATABASE; break;  //    l.append( QString("Database") );
//    case 4: w = createMeterLogsPage(   gHelper,  this) ; break; //    l.append( QString("Meter logs") );
//    case 5: w = new KtsConnectWdgt(     gHelper,  this); break;
    case 4: w = createPageLog(gHelper,  this)  ; break;   //    l.append( QString("State")                   );

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
        const TabName2icosPaths ttn = CreateToolBar::getTabs().at(row);

        w->setWindowTitle(ttn.tabName);
        w->setWindowIcon(QIcon(ttn.path));


//        connect(this, SIGNAL(killMyChild()), w, SLOT(deleteLater()) );
//        connect(this, SIGNAL(onOperationNError(int)), w, SLOT(onOperationNError(int)) );
//        w->setRwCommand(MatildaDeviceTree::getPageCanRead().at(row), MatildaDeviceTree::getPageCanWrite().at(row));

        w->setRwCommand(readCommand, writeCommand);
        w->setHasDataFromRemoteDevice();
        w->setupGlobalLblMessage(ui->lblPageMess);
        QWidget *sa = StackWidgetHelper::addWdgtWithScrollArea(this, w, tabData);
        ui->stackedWidget->addWidget(sa);
        ui->stackedWidget->setCurrentWidget(sa);
    }
}

//---------------------------------------------------------------------

void QcMainWindow::createToolBar()
{
    CreateToolBar *c = new CreateToolBar(this);
    connect(c, SIGNAL(onActivateThisWdgt(QString)), this, SLOT(onActivateThisWdgt(QString)) );
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

    m->moveToThread(t);

    connect(this, &QcMainWindow::destroyed, m, &ZbyratorProcessManager::deleteLater);
    connect(m, &ZbyratorProcessManager::destroyed, t, &QThread::quit);
    connect(t, &QThread::finished, t, &QThread::deleteLater);

    connect(t, &QThread::started, m, &ZbyratorProcessManager::onThreadStarted);

    QTimer::singleShot(1111, t, SLOT(start()));


}

//---------------------------------------------------------------------

void QcMainWindow::createMeterManager()
{

    MeterManager *zbyrator = new MeterManager(true, ZbyrConnSett());

    QThread *thread = new QThread;


    zbyrator->moveToThread(thread);
    connect(thread, SIGNAL(started()), zbyrator, SLOT(onThreadStarted()));

//    connect(extSocket, &ZbyratorSocket::appendDbgExtData, this, &ZbyratorManager::appendDbgExtData );



//    connect(zbyrator, &MeterManager::command2extensionClient, extSocket, &ZbyratorSocket::command2extensionClient   );
//    connect(zbyrator, &MeterManager::onAboutZigBee          , extSocket, &ZbyratorSocket::sendAboutZigBeeModem      );

    metersListMedium = new ZbyrMeterListMedium(this);

    connect(metersListMedium, SIGNAL(command4dev(quint16,QString))    , zbyrator, SIGNAL(command4devStr(quint16,QString)) );
    connect(metersListMedium, SIGNAL(command4dev(quint16,QVariantMap)), zbyrator, SIGNAL(command4dev(quint16,QVariantMap)) );

    connect(metersListMedium, &ZbyrMeterListMedium::setPbReadEnableDisable, guiHelper, &GuiHelper::setPbReadEnableDisableSlot);
    connect(metersListMedium, &ZbyrMeterListMedium::updateHashSn2meter, guiHelper, &GuiHelper::updateHashSn2meter);
    connect(metersListMedium, &ZbyrMeterListMedium::data2dbMedium      , guiHelper, &GuiHelper::updateSettDateMaskAndDotPos);

    connect(metersListMedium, &ZbyrMeterListMedium::setElectricityPowerCenters, guiHelper, &GuiHelper::setElectricityPowerCenters);
    connect(metersListMedium, &ZbyrMeterListMedium::setWaterPowerCenters, guiHelper, &GuiHelper::setWaterPowerCenters);
    connect(metersListMedium, &ZbyrMeterListMedium::pbStopAnimateClick, guiHelper, &GuiHelper::pbStopAnimateClick);
    guiHelper->setObjectName("QcMainWindow");

    connect(metersListMedium, &ZbyrMeterListMedium::appendAppLog        , this, &QcMainWindow::appendShowMessPlain );

    connect(metersListMedium, &ZbyrMeterListMedium::onReloadAllMeters2zbyrator, zbyrator, &MeterManager::onReloadAllMetersIgnoreOff    );
    connect(metersListMedium, &ZbyrMeterListMedium::onConfigChanged     , zbyrator, &MeterManager::onConfigChanged      );
    connect(metersListMedium, &ZbyrMeterListMedium::sendMeAlistOfMeters , zbyrator, &MeterManager::sendMeAlistOfMeters  );

    connect(metersListMedium, &ZbyrMeterListMedium::setThisIfaceSett    , zbyrator, &MeterManager::setThisIfaceSett     );
//    connect(metersListMedium, &ZbyrMeterListMedium::setPollSaveSettings , zbyrator, &MeterManager::setPollSaveSettings  ); zbyrator has it's own
    connect(metersListMedium, SIGNAL(setPollSaveSettings(quint16,quint16,bool,bool,bool,qint32,qint32,qint32,bool)), zbyrator, SLOT(reloadPollSettings()));

    connect(metersListMedium, &ZbyrMeterListMedium::giveMeYourCache     , zbyrator, &MeterManager::giveMeYourCache      );
    connect(metersListMedium, &ZbyrMeterListMedium::killUconTasks       , zbyrator, &MeterManager::killUconsTasks       );
    connect(metersListMedium, &ZbyrMeterListMedium::setIgnoreCycles     , zbyrator, &MeterManager::setIgnoreCycles      );
    connect(metersListMedium, &ZbyrMeterListMedium::setOnlyGlobalConnection     , zbyrator, &MeterManager::setOnlyGlobalConnection      );

    connect(metersListMedium, &ZbyrMeterListMedium::reloadSavedSleepProfiles, zbyrator, &MeterManager::reloadSavedSleepProfiles      );
    connect(metersListMedium, &ZbyrMeterListMedium::reloadSettings, zbyrator, &MeterManager::reloadSettings);

    connect(metersListMedium, SIGNAL(showMess(QString)), this, SLOT(showMess(QString)) );

    connect(zbyrator, SIGNAL(checkThisMeterInfo(UniversalMeterSett)), metersListMedium, SIGNAL(onReloadAllMeters2zbyrator()) );

    connect(zbyrator, &MeterManager::onAllMeters                , metersListMedium, &ZbyrMeterListMedium::onAllMetersSlot           );
    connect(zbyrator, &MeterManager::onAllMeters                , metersListMedium, &ZbyrMeterListMedium::onAllMeters               );
    connect(zbyrator, &MeterManager::onAlistOfMeters            , metersListMedium, &ZbyrMeterListMedium::onAlistOfMeters           );
    connect(zbyrator, &MeterManager::ifaceLogStr                , metersListMedium, &ZbyrMeterListMedium::ifaceLogStr               );
    connect(zbyrator, &MeterManager::ifaceLogStrNonBuf    , metersListMedium, &ZbyrMeterListMedium::ifaceLogStrFromZbyrator               );

    connect(zbyrator, &MeterManager::appendMeterData            , metersListMedium, &ZbyrMeterListMedium::appendMeterData           );
    connect(zbyrator, &MeterManager::onConnectionStateChanged   , metersListMedium, &ZbyrMeterListMedium::onConnectionStateChanged  );
    connect(zbyrator, &MeterManager::onUconStartPoll            , metersListMedium, &ZbyrMeterListMedium::onUconStartPoll           );
    connect(zbyrator, &MeterManager::onReadWriteOperation       , metersListMedium, &ZbyrMeterListMedium::onReadWriteOperation      );

    connect(zbyrator, &MeterManager::onStatisticChanged         , metersListMedium, &ZbyrMeterListMedium::onStatisticChanged        );
    connect(zbyrator, &MeterManager::onAllStatHash              , metersListMedium, &ZbyrMeterListMedium::onAllStatHash             );
    connect(zbyrator, &MeterManager::onTaskTableChanged         , metersListMedium, &ZbyrMeterListMedium::onTaskTableChanged        );
    connect(zbyrator, &MeterManager::onTaskCanceled             , metersListMedium, &ZbyrMeterListMedium::onTaskCanceled            );
    connect(zbyrator, &MeterManager::meterRelayStatus           , metersListMedium, &ZbyrMeterListMedium::meterRelayStatus          );
    connect(zbyrator, &MeterManager::meterRelayStatus           , metersListMedium, &ZbyrMeterListMedium::add2fileMeterRelayStatus  );

    connect(zbyrator, &MeterManager::meterDateTimeDstStatus     , metersListMedium, &ZbyrMeterListMedium::meterDateTimeDstStatus    );
    connect(zbyrator, &MeterManager::waterMeterSchedulerStts    , metersListMedium, &ZbyrMeterListMedium::waterMeterSchedulerStts   );

    connect(zbyrator, &MeterManager::appendAppOut, this, &QcMainWindow::appendShowMessPlain);


    connect(zbyrator, &MeterManager::command2extensionClient, metersListMedium, &ZbyrMeterListMedium::command2extensionClient   );
    connect(zbyrator, &MeterManager::onAboutZigBee          , metersListMedium, &ZbyrMeterListMedium::onAboutZigBee      );
    connect(zbyrator, &MeterManager::relayStatusChanged     , metersListMedium, &ZbyrMeterListMedium::relayStatusChanged        );

    guiHelper->managerEnDisBttn.pbReadDis = guiHelper->managerEnDisBttn.pbWriteDis =false;

    connect(zbyrator, &MeterManager::onConnectionStateChanged, guiHelper, &GuiHelper::setPbWriteEnableDisableSlot);// ReadEnableDisableSlot);


    connect(this, &QcMainWindow::reloadSettings2ucEmulator, metersListMedium, &ZbyrMeterListMedium::reloadSettings);


    connect(guiHelper, &GuiHelper::setDateMask, metersListMedium, &ZbyrMeterListMedium::setDateMask);
    connect(guiHelper, &GuiHelper::setDotPos, metersListMedium, &ZbyrMeterListMedium::setDotPos);


    metersListMedium->importGroups2metersFile();
    QTimer::singleShot(1111, thread, SLOT(start()) );

}

//---------------------------------------------------------------------

void QcMainWindow::createMatildaBBBcover()
{
    const QString nameStr = "Matilda";
    const bool verbose =
        #ifdef Q_OS_LINUX
            true
        #else
            false
        #endif
            ;
    MatildaBBBcover *c = new MatildaBBBcover(nameStr, verbose);
    QThread *t = new QThread;
    c->moveToThread(t);

    connect(this, SIGNAL(destroyed(QObject*)), c, SLOT(deleteLater()));
    connect(c, SIGNAL(destroyed(QObject*)), t, SLOT(quit()));
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));

    connect(t, SIGNAL(started()), c, SLOT(onThreadStarted()));

    connect(this, &QcMainWindow::reloadSettings2ucEmulator, c, &MatildaBBBcover::reloadSettings2ucEmulator);

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

    connect(this, &QcMainWindow::appendShowMess, w, &StartExchange::appendShowMessPlain);
    connect(this, &QcMainWindow::appendShowMessPlain, w, &StartExchange::appendShowMessPlain);

    connect(w, SIGNAL(pageReady()), this, SIGNAL(initDone()));

    return w;
}

//---------------------------------------------------------------------

MatildaConfWidget *QcMainWindow::createElectricityMeterListWdgt(GuiHelper *gHelper, QWidget *parent)
{

    MeterListWdgt *w = new MeterListWdgt(MeterListWdgt::elMeter(), gHelper,  parent);
    w->enableLazyActivation();
    connect(w, &MeterListWdgt::onReloadAllMeters, metersListMedium, &ZbyrMeterListMedium::onReloadAllMeters2zbyrator);
    connect(w, &MeterListWdgt::meterModelChanged, metersListMedium, &ZbyrMeterListMedium::meterElectricityModelChanged);
//    connect(metersListMedium, &ZbyrMeterListMedium::setElectricityMeterListPageSett, w, &MeterListWdgt::setPageSett);
    connect(metersListMedium, SIGNAL(setElectricityMeterListPageSett(MyListStringList,QVariantMap,QStringList,QStringList,bool)), w, SLOT(setPageSett(MyListStringList,QVariantMap,QStringList,QStringList,bool)) );

    return w;


}
//---------------------------------------------------------------------

MatildaConfWidget *QcMainWindow::createWaterMeterListWdgt(GuiHelper *gHelper, QWidget *parent)
{

    MeterListWdgt *w = new MeterListWdgt(MeterListWdgt::waterMeter(), gHelper,  parent);
    w->enableLazyActivation();
    connect(w, &MeterListWdgt::onReloadAllMeters, metersListMedium, &ZbyrMeterListMedium::onReloadAllMeters2zbyrator);
    connect(w, &MeterListWdgt::meterModelChanged, metersListMedium, &ZbyrMeterListMedium::meterWaterModelChanged);
//    connect(metersListMedium, &ZbyrMeterListMedium::setElectricityMeterListPageSett, w, &MeterListWdgt::setPageSett);
    connect(metersListMedium, SIGNAL(setWaterMeterListPageSett(MyListStringList,QVariantMap,QStringList,QStringList,bool)), w, SLOT(setPageSett(MyListStringList,QVariantMap,QStringList,QStringList,bool)) );

    return w;
}
//---------------------------------------------------------------------

MatildaConfWidget *QcMainWindow::createPageLog(GuiHelper *gHelper, QWidget *parent)
{
    SmplPteWdgt *w = new SmplPteWdgt(tr("Log") , true, true, gHelper,  false, parent);
    connect(w, &SmplPteWdgt::giveMeYourCache, metersListMedium, &ZbyrMeterListMedium::giveMeYourCache);
    connect(metersListMedium, &ZbyrMeterListMedium::ifaceLogStr, w, &SmplPteWdgt::appendPteText);
    return w;
}

MatildaConfWidget *QcMainWindow::createDatabasePage(GuiHelper *gHelper, QWidget *parent)
{
    DatabaseWdgtV2 *w = new DatabaseWdgtV2(gHelper,  parent);


    connect(w, SIGNAL(data2dbMedium(quint16,QVariant)), metersListMedium, SIGNAL(data2dbMedium(quint16,QVariant)) );
    connect(w, &DatabaseWdgtV2::stopDbReading, metersListMedium, &ZbyrMeterListMedium::stopReadDatabase);

    connect(metersListMedium, SIGNAL(setLblWaitTxtDatabase(QString)), w, SIGNAL(setLblWaitTxtDatabase(QString)));
    connect(metersListMedium, SIGNAL(appendDataDatabase(QVariantHash)), w, SLOT(setPageSett(QVariantHash)));
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

        connect(w, &ZbyratorOptions::reloadSettings2ucEmulator, this, &QcMainWindow::reloadSettings2ucEmulator);

        addWdgt2stackWdgt(w, WDGT_TYPE_OPTIONS, false, "", "");// tr("Options"), ":/katynko/svg/applications-system.svg");
    }
}

void QcMainWindow::on_actionAbout_triggered()
{
    showMess(AppVersion::getVersion());
}
