#include "startexchange.h"
#include "ui_startexchange.h"
#include "gui-src/stackwidgethelper.h"
#include "gui-src/treemodel.h"

#include "startpagepoll.h"
#include "relaywdgt.h"
#include "metersdatetime.h"
#include "setmeteraddress.h"
#include "checkconnectiontoolwdgt.h"
#include "zbyratorservice.h"
#include "zbyrifacesett.h"
#include "info-pgs/zbyratortasks.h"
#include "info-pgs/statisticofexchangewdgt.h"
#include "prepaid-pgs/ifaceindicationwdgt.h"
#include "src/zbyratortasksmedium.h"
#include "zbyrator-src/src/startexchangehelper.h"
#include "zbyrator-water/watersleepscheduler.h"
#include "src/widgets/ifacelabel.h"

StartExchange::StartExchange(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    MatildaConfWidget(lDevInfo, gHelper, gSett4all, parent),
    ui(new Ui::StartExchange)
{
    ui->setupUi(this);
    ui->trDevOperation->setEnabled(false);

}
//-----------------------------------------------------------------------------------------------
StartExchange::~StartExchange()
{
    delete ui;
}

//-----------------------------------------------------------------------------------------------
void StartExchange::initPage()
{

    guiHelper = new GuiHelper(this);

    guiHelper->initObj();
    guiHelper->guiSett = gHelper->guiSett;
    guiHelper->lDevInfo = gHelper->lDevInfo;
    guiHelper->cacheHelper = gHelper->cacheHelper;

    guiHelper->stackedWidget = gHelper->stackedWidget;
    guiHelper->parentWidget = gHelper->parentWidget;
    connect(guiHelper, SIGNAL(addWdgt2stackWdgt(QWidget*,int,bool,QString,QString)), gHelper, SIGNAL(addWdgt2stackWdgt(QWidget*,int,bool,QString,QString)) );
    connect(guiHelper, SIGNAL(showMess(QString)), gHelper, SIGNAL(showMess(QString)));
    connect(guiHelper, SIGNAL(showMessCritical(QString)), gHelper, SIGNAL(showMessCritical(QString)) );
    connect(ui->swDeviceOperations, SIGNAL(currentChanged(int)), this, SLOT(onSwDevicesCurrIndxChanged()) );
    ui->wdgtReadButton->setEnabled(false);

    connect(gHelper, SIGNAL(setPbWriteEnableDisable(bool)), ui->pbRead, SLOT(setDisabled(bool)));
    ui->pbRead->setDisabled(gHelper->managerEnDisBttn.pbWriteDis);

    guiHelper->stackedWidget = ui->swDeviceOperations;
    guiHelper->parentWidget = ui->swDeviceOperations;
    ui->trDevOperation->setContextMenuPolicy(Qt::CustomContextMenu);

    TreeModel * modelDevOptTree = new TreeModel(this);
    ui->trDevOperation->setModel(modelDevOptTree);
    //ui->trDevOperation->setRootIndex();

    modelDevOptTree->setDefSize4itms(QSize(ui->trDevOperation->width(), ui->pbStop->height() * 1.2));

    guiHelper->managerEnDisBttn.pbReadDis = false;
    connect(metersListMedium, &ZbyrMeterListMedium::onConnectionStateChanged, guiHelper, &GuiHelper::setPbReadEnableDisableSlot);
    connect(metersListMedium, &ZbyrMeterListMedium::setLblWaitTxt, ui->lblOperationInProgress, &QLabel::setText);

    ui->pbStop->setEnabled(guiHelper->managerEnDisBttn.pbReadDis);
    connect(metersListMedium, &ZbyrMeterListMedium::onConnectionStateChanged, ui->pbStop, &QPushButton::setEnabled);
    connect(guiHelper, SIGNAL(pbStopAnimateClick()), ui->pbStop, SLOT(animateClick()));

    connect(ui->pbStop, SIGNAL(clicked(bool)), metersListMedium, SIGNAL(killUconTasks()) );


    connect(metersListMedium, SIGNAL(setLblWaitTxt(QString)), this, SLOT(updateScrollAreaHeight()) );
    connect(metersListMedium, &ZbyrMeterListMedium::updateHashSn2meter, guiHelper, &GuiHelper::updateHashSn2meter);

    QStringList listIcos, chListNames;


    //, QStringList chListNames, QStringList listIcos, int buttonH)


    const QStringList chListData = StartExchangeHelper::getChListData(listIcos, chListNames);


    for(int i = 0, imax = listIcos.size(); i < imax; i++)
        modelDevOptTree->appendItem(chListNames.at(i), chListData.at(i), listIcos.at(i));

    ui->trDevOperation->setCurrentIndex(modelDevOptTree->index(0,0));
    lastWdgtAccessibleName = chListData.first();

//    showWdgtByName(chListData.first(), chListNames.first(), QIcon(listIcos.first()));
    QTimer::singleShot(333, this, SLOT(unlockWdgts()) );



    if(true){
        IfaceIndicationWdgt *w = new IfaceIndicationWdgt(gSett4all->font4log, this);
        ui->hl4wdgt->addWidget(w);
        w->disableLogMode(true);
        connect(metersListMedium, SIGNAL(onReadWriteOperation(bool)), w, SLOT(onReadWriteOperation(bool)) );
    }

    if(true){
        IfaceLabel *l = new IfaceLabel(this);
        connect(metersListMedium, &ZbyrMeterListMedium::onIfaceSett, l, &IfaceLabel::onIfaceSett);
        connect(l, &IfaceLabel::onTextChanged, ui->lblIface, &QLabel::setText);
        ui->lblIface->setText(l->text());
        QTimer::singleShot(2222, metersListMedium, SLOT(sendMeIfaceSett()));
    }


}
//-----------------------------------------------------------------------------------------------
void StartExchange::unlockWdgts()
{
    ui->trDevOperation->setEnabled(true);
    const QString named = lastWdgtAccessibleName;
    lastWdgtAccessibleName.clear();
    showWdgtByNameData(named);
    updateScrollAreaHeight();
}
//-----------------------------------------------------------------------------------------------
void StartExchange::showWdgtByName(const QString &wdgtAccessibleName, const QString &wdgtTitle)
{
    showWdgtByName(wdgtAccessibleName, wdgtTitle, QIcon());

}
//-----------------------------------------------------------------------------------------------
void StartExchange::showWdgtByName(const QString &wdgtAccessibleName, const QString &wdgtTitle, const QIcon &itemIcon)
{
    if(!ui->trDevOperation->isEnabled())
        return;

    if(wdgtAccessibleName != lastWdgtAccessibleName && wdgtAccessibleName != "Interface"){
//        gHelper->closeYourPopupsSlot();
        lastWdgtAccessibleName = wdgtAccessibleName;
    }
    if(GuiHelper::stackContainsThisWdgt(ui->swDeviceOperations, wdgtAccessibleName, true, 0)){
        MatildaConfWidget *w = currentMatildaWidget();
        if(w){
            w->pageActivated();
//            w->setupGlobalLblMessage(ui->lblPageMess);
        }
    }else{
        addWdgt2devStack(wdgtAccessibleName, wdgtTitle, itemIcon);
    }

}

void StartExchange::showLastWdgt()
{
    showWdgtByNameData(lastWdgtAccessibleName);

}

void StartExchange::updateScrollAreaHeight()
{
    ui->scrollArea->setMaximumHeight( ui->pbStop->height() * 2.3);// (title.isEmpty() ? 1.3 : 2.3));

}

void StartExchange::appendShowMessPlain(QString m)
{
    const QString s = m.split("\n", QString::SkipEmptyParts).last();

    if(!s.isEmpty()){
        const QStringList l = s.split(": ");
        if(l.size() > 1)
            ui->lblCurrentMeter->setText(l.mid(1).join(": "));
    }
}


//-----------------------------------------------------------------------------------------------
void StartExchange::on_tbIfaceSett_clicked()
{
    //open iface settings widget

    showWdgtByNameData("Interface");


//    TreeModel * modelDevOptTree = new TreeModel(this);

}

//-----------------------------------------------------------------------------------------------
MatildaConfWidget *StartExchange::currentMatildaWidget()
{
    QScrollArea *sa = qobject_cast<QScrollArea*>(ui->swDeviceOperations->currentWidget());
    if(sa){
        MatildaConfWidget *w = qobject_cast<MatildaConfWidget*>(sa->widget());
        if(w)
            return w;
    }
    return 0;
}
//-----------------------------------------------------------------------------------------------
MatildaConfWidget *StartExchange::createStartPagePoll(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent)
{
    StartPagePoll *w = new StartPagePoll(lDevInfo, gHelper, gSett4all, parent);
    w->metersListMedium = metersListMedium;
    connect(metersListMedium, SIGNAL(appendData2model(QVariantHash)), w, SLOT(setPageSett(QVariantHash)) );

    connect(metersListMedium, &ZbyrMeterListMedium::onUpdatedSavedList, w, &StartPagePoll::onUpdatedSavedList);

    connect(w, &StartPagePoll::setIgnoreCycles  , metersListMedium, &ZbyrMeterListMedium::setIgnoreCycles);

    connect(w, &StartPagePoll::onReloadAllMeters, metersListMedium, &ZbyrMeterListMedium::onReloadAllMeters );
    connect(w, &StartPagePoll::onPollStarted    , metersListMedium, &ZbyrMeterListMedium::onPollStarted     );

    connect(w, SIGNAL(command4dev(quint16,QString))    , metersListMedium, SLOT(command4devSlot(quint16,QString)) );
    connect(w, SIGNAL(command4dev(quint16,QVariantMap)), metersListMedium, SLOT(command4devSlot(quint16,QVariantMap)) );


    return w;
}
//-----------------------------------------------------------------------------------------------
MatildaConfWidget *StartExchange::createWaterSleepSchedulerWdgt(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent)
{
    WaterSleepScheduler *w = new WaterSleepScheduler(lDevInfo, gHelper, gSett4all, parent);

    connect(w, &WaterSleepScheduler::onReloadAllMeters, metersListMedium, &ZbyrMeterListMedium::onReloadAllMeters);
    connect(w, &WaterSleepScheduler::setLastPageId, metersListMedium, &ZbyrMeterListMedium::setLastPageId);
    connect(w, SIGNAL(command4dev(quint16,QVariantMap)), metersListMedium, SLOT(command4devSlot(quint16,QVariantMap)) );

    connect(w, &WaterSleepScheduler::reloadSavedSleepProfiles, metersListMedium, &ZbyrMeterListMedium::reloadSavedSleepProfiles);
    connect(metersListMedium, &ZbyrMeterListMedium::waterMeterSchedulerStts, w, &WaterSleepScheduler::waterMeterSchedulerStts);
    connect(metersListMedium, SIGNAL(setWaterMeterSchedulerPageSett(MyListStringList,QVariantMap,QStringList,QStringList,bool)), w, SLOT(setPageSett(MyListStringList,QVariantMap,QStringList,QStringList,bool)) );
    return w;

}
//-----------------------------------------------------------------------------------------------
MatildaConfWidget *StartExchange::createRelayWdgt(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent)
{
    RelayWdgt *w = new RelayWdgt(lDevInfo, gHelper, gSett4all, parent);
//    connect(metersListMedium, &ZbyrMeterListMedium::setRelayPageSett, w, &RelayWdgt::setp);
    connect(metersListMedium, SIGNAL(setRelayPageSett(MyListStringList,QVariantMap,QStringList,QStringList,bool)), w, SLOT(setPageSett(MyListStringList,QVariantMap,QStringList,QStringList,bool)) );
    connect(metersListMedium, &ZbyrMeterListMedium::meterRelayStatus, w, &RelayWdgt::meterRelayStatus);
    connect(w, &RelayWdgt::onReloadAllMeters, metersListMedium, &ZbyrMeterListMedium::onReloadAllMeters);
    connect(w, &RelayWdgt::setLastPageId, metersListMedium, &ZbyrMeterListMedium::setLastPageId);
    connect(w, SIGNAL(command4dev(quint16,QVariantMap)), metersListMedium, SLOT(command4devSlot(quint16,QVariantMap)) );

    return w;
}

//-----------------------------------------------------------------------------------------------

MatildaConfWidget *StartExchange::createMetersDateTime(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent)
{
    MetersDateTime *w = new MetersDateTime(lDevInfo, gHelper, gSett4all, parent);
    connect(metersListMedium, SIGNAL(setDateTimePageSett(MyListStringList,QVariantMap,QStringList,QStringList,bool)), w, SLOT(setPageSett(MyListStringList,QVariantMap,QStringList,QStringList,bool)) );
    connect(w, &MetersDateTime::onReloadAllMeters, metersListMedium, &ZbyrMeterListMedium::onReloadAllMeters);
    connect(w, SIGNAL(command4dev(quint16,QVariantMap)), metersListMedium, SLOT(command4devSlot(quint16,QVariantMap)) );
    connect(w, &MetersDateTime::setLastPageId           , metersListMedium, &ZbyrMeterListMedium::setLastPageId     );
    connect(metersListMedium, &ZbyrMeterListMedium::meterDateTimeDstStatus, w, &MetersDateTime::meterDateTimeDstStatus);

    return w;
}
//-----------------------------------------------------------------------------------------------
MatildaConfWidget *StartExchange::createZbyrIfaceSett(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent)
{
    ZbyrIfaceSett * w = new ZbyrIfaceSett(lDevInfo, gHelper, gSett4all, parent);

    connect(metersListMedium, SIGNAL(setIfaceSett(QVariantHash)), w, SLOT(setPageSett(QVariantHash)) );
    connect(w, SIGNAL(setNewSettings(QVariantHash)), metersListMedium, SLOT(setNewSettings(QVariantHash)));
    connect(w, SIGNAL(sendMeIfaceSett()), metersListMedium, SLOT(sendMeIfaceSett()) );
    connect(w, SIGNAL(setNewSettings(QVariantHash)), this, SLOT(showLastWdgt()) );
    connect(w, SIGNAL(closeThisPage())              , this, SLOT(showLastWdgt()));

    connect(w, SIGNAL(sendMeTheTcpHistory(QLineEdit*))      , metersListMedium, SLOT(openTcpServerDlg(QLineEdit*)) );
    connect(w, SIGNAL(sendMeTheM2mHistory(QLineEdit*))      , metersListMedium, SLOT(openM2mDlg(QLineEdit*)));
    connect(w, SIGNAL(openEditMacProfileWdgt(bool,QLineEdit*)), this, SIGNAL(openEditMacProfileWdgt(bool,QLineEdit*)));

    return w;
}
//-----------------------------------------------------------------------------------------------
MatildaConfWidget *StartExchange::createStatisticWdgt(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent)
{
    StatisticOfExchangeWdgt *w = new StatisticOfExchangeWdgt( lDevInfo, gHelper, gSett4all, parent);
    connect(w, SIGNAL(onPollCodeChanged(QVariantHash)), metersListMedium, SLOT(onPollCodeChangedStat(QVariantHash)) );
    connect(metersListMedium, SIGNAL(onStatisticChanged(QString,QStringList))       , w, SLOT(onStatisticChanged(QString,QStringList))  );
    connect(metersListMedium, SIGNAL(setStatisticOfExchangePageSett(QVariantHash))  , w, SLOT(setPageSett(QVariantHash))                );
    return w;

}

MatildaConfWidget *StartExchange::createZbyratorTaskWdgt(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent)
{

    ZbyratorTasksMedium *m = new ZbyratorTasksMedium;
    QThread *t = new QThread;
    m->moveToThread(t);
    connect(this, SIGNAL(destroyed(QObject*)), m, SLOT(deleteLater()));
    connect(m, SIGNAL(destroyed(QObject*)), t, SLOT(quit()) );
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()) );
    connect(t, SIGNAL(started()), m, SLOT(onThreadStarted()) );

    ZbyratorTasks *w = new ZbyratorTasks(      lDevInfo, gHelper, gSett4all, parent);
    connect(w, SIGNAL(onZbyratorConfigChanged(quint16,QVariant)), metersListMedium, SIGNAL(onConfigChanged(quint16,QVariant)));

    connect(metersListMedium, SIGNAL(onTaskTableChanged()), m, SLOT(onTaskTableChanged()));
    connect(w, SIGNAL(onPageCanReceiveData()), m, SLOT(onPageReady()));

    connect(m, SIGNAL(setZbyratorTasksPageSett(MyListStringList,QVariantMap,QStringList,QStringList,bool)), w, SLOT(setPageSett(MyListStringList,QVariantMap,QStringList,QStringList,bool)));

    t->start();

    return w;


}
//-----------------------------------------------------------------------------------------------
void StartExchange::showWdgtByNameData(const QString &named)
{
    QStringList listIcos, chListNames;


    //, QStringList chListNames, QStringList listIcos, int buttonH)


    const QStringList chListData = StartExchangeHelper::getChListData(listIcos, chListNames);

    const int row = chListData.indexOf(named);
    if(row < 0)
        return;
//QString("Poll;Relay;Queue;Statistic of the exchange;Date and time;Meter address;Check Connection Tool;Other;Interface").split(";");

    showWdgtByName(chListData.at(row), chListNames.at(row), QIcon(listIcos.at(row)));
//ui->trDevOperation->model()
    TreeModel *modelDevOptTree { static_cast<TreeModel *>(ui->trDevOperation->model())};
    if(modelDevOptTree)
        ui->trDevOperation->setCurrentIndex(modelDevOptTree->index(row,0));


}

//-----------------------------------------------------------------------------------------------

void StartExchange::on_trDevOperation_clicked(const QModelIndex &index)
{
    if(index.isValid()){
//        if(true){
//            MatildaConfWidget *w = currentMatildaWidget();
//            if(w)
//                w->disableGlobalLblMessage(ui->lblPageMess);
//        }


        const QString s = index.data(Qt::UserRole).toString();

        if(!s.isEmpty()){
            //real page name
            showWdgtByName(s, index.data(Qt::DisplayRole).toString(), index.data(Qt::DecorationRole).value<QIcon>());


        }
    }

}

void StartExchange::addWdgt2devStack(const QString &realPageName, const QString &wdgtTitle, const QIcon &itemIcon)
{
    QStringList listIcos, chListNames;


    //, QStringList chListNames, QStringList listIcos, int buttonH)


    const QStringList chListData = StartExchangeHelper::getChListData(listIcos, chListNames);

//    return QString("Poll;Scheduler for water meters;Relay;Queue;Statistic of the exchange;Date and time;Meter address;Check Connection Tool;Other;Interface").split(";");

    MatildaConfWidget *w = 0;

    bool hasReadButton = false;
    const int row = chListData.indexOf(realPageName);
    switch(row){
    case 0: w = createStartPagePoll(    lDevInfo, guiHelper, gSett4all, this); hasReadButton = true; break;
    case 1: w = createWaterSleepSchedulerWdgt(        lDevInfo, guiHelper, gSett4all, this); hasReadButton = true; break;

    case 2: w = createRelayWdgt(        lDevInfo, guiHelper, gSett4all, this); hasReadButton = true; break;

    case 3: w = createZbyratorTaskWdgt( lDevInfo, guiHelper, gSett4all, this); break;
    case 4: w = createStatisticWdgt( lDevInfo, guiHelper, gSett4all, this); break;

    case 5: w = createMetersDateTime(   lDevInfo, guiHelper, gSett4all, this); hasReadButton = true; break;
    case 6: w = new SetMeterAddress(    lDevInfo, guiHelper, gSett4all, this); break;

    case 7: w = new CheckConnectionToolWdgt( lDevInfo, guiHelper, gSett4all, this); break;
    case 8: w = new ZbyratorService(    lDevInfo, guiHelper, gSett4all, this); break;

    case 9: w = createZbyrIfaceSett(      lDevInfo, guiHelper, gSett4all, this); break;

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
        w->setWindowTitle(wdgtTitle);
        w->setWindowIcon(itemIcon);

//        connect(this, SIGNAL(onCOMMAND2GUI(quint16,QString)), w, SLOT(onCOMMAND2GUI(quint16,QString)) );
//        connect(this, SIGNAL(onCOMMAND2GUI(quint16,QVariantHash)), w, SLOT(onCOMMAND2GUI(quint16,QVariantHash)) );

//        connect(w, SIGNAL(onCOMMAND2GUIready()), this, SIGNAL(onCOMMAND2GUIready()) );

//        connect(this, SIGNAL(killMyChild()), w, SLOT(deleteLater()) );
//        connect(this, SIGNAL(onOperationNError(int)), w, SLOT(onOperationNError(int)) );
        w->setRwCommand( hasReadButton ? (10 + row) : 0, 1);// MatildaDeviceTree::getPageCanRead().at(row), MatildaDeviceTree::getPageCanWrite().at(row));

        w->setAccessibleName(realPageName);
        qDebug() << "realPageName " << row << realPageName << wdgtTitle ;

//        w->setupGlobalLblMessage(ui->lblPageMess);
        QWidget *sa = StackWidgetHelper::addWdgtWithScrollArea(this, w, realPageName);
        ui->swDeviceOperations->addWidget(sa);
        ui->swDeviceOperations->setCurrentWidget(sa);
    }

}


void StartExchange::on_pbStop_clicked()
{
    ui->pbStop->setEnabled(false);
}

void StartExchange::on_pbRead_clicked()
{
    MatildaConfWidget *w = currentMatildaWidget();

    if(w){
        ui->pbRead->setEnabled(false);
        bool ok;
        QString mess;
        w->getPageSett4read(ok, mess);

        QTimer::singleShot(555, this, SLOT(checkPbReadEnabled()));
    }
}

void StartExchange::checkPbReadEnabled()
{
    ui->pbRead->setDisabled(guiHelper->managerEnDisBttn.pbWriteDis);
}

void StartExchange::onSwDevicesCurrIndxChanged()
{
    MatildaConfWidget *w = currentMatildaWidget();

    if(w){
        const int readCommand = w->getReadCommand();
//        const int writeCommand = w->getWriteCommand();

        ui->wdgtReadButton->setEnabled(readCommand > 0);

    }
}
