#include "startexchange.h"
#include "ui_startexchange.h"
#include "src/shared/stackwidgethelper.h"
#include "src/shared/treemodel.h"

#include "startpagepoll.h"
#include "relaywdgt.h"
#include "metersdatetime.h"
#include "setmeteraddress.h"
#include "checkconnectiontoolwdgt.h"
#include "zbyratorservice.h"
#include "zbyrifacesett.h"
#include "dataconcetrator-pgs/zbyratortasks.h"
#include "info-pgs/statisticofexchangewdgt.h"

StartExchange::StartExchange(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    MatildaConfWidget(lDevInfo, gHelper, gSett4all, parent),
    ui(new Ui::StartExchange)
{
    ui->setupUi(this);
}
//-----------------------------------------------------------------------------------------------
StartExchange::~StartExchange()
{
    delete ui;
}

QStringList StartExchange::getChListData(QStringList &listIcos, QStringList &chListNames)
{
    chListNames = tr("Poll;Relay;Queue;Statistic of the exchange;Date and time;Meter address;Check Connection Tool;Other;Interface").split(";");
    listIcos.clear();
    listIcos.append(":/katynko/svg/edit-table-insert-row-below.svg");
    listIcos.append(":/katynko/svg/format-disconnect-node.svg");
    listIcos.append(":/katynko/svg/format-list-ordered.svg");
    listIcos.append(":/katynko/svg/view-statistics.svg");
    listIcos.append(":/katynko/svg/chronometer.svg");
    listIcos.append(":/katynko/svg/address-book-new.svg");
    listIcos.append(":/katynko/svg/irc-voice.svg");
    listIcos.append(":/katynko/svg/irc-voice.svg");
    listIcos.append(":/katynko/svg/network-disconnect.svg");

    return QString("Poll;Relay;Queue;Statistic of the exchange;Date and time;Meter address;Check Connection Tool;Other;Interface").split(";");

}
//-----------------------------------------------------------------------------------------------
void StartExchange::initPage()
{

    ui->trDevOperation->setContextMenuPolicy(Qt::CustomContextMenu);

    TreeModel * modelDevOptTree = new TreeModel(this);
    ui->trDevOperation->setModel(modelDevOptTree);
    //ui->trDevOperation->setRootIndex();

    modelDevOptTree->setDefSize4itms(QSize(ui->trDevOperation->width(), ui->pbStop->height() * 1.2));



    QStringList listIcos, chListNames;


    //, QStringList chListNames, QStringList listIcos, int buttonH)


    const QStringList chListData = getChListData(listIcos, chListNames);


    for(int i = 0, imax = listIcos.size(); i < imax; i++)
        modelDevOptTree->appendItem(chListNames.at(i), chListData.at(i), listIcos.at(i));

    ui->trDevOperation->setCurrentIndex(modelDevOptTree->index(0,0));
    showWdgtByName(chListData.first(), chListNames.first(), QIcon(listIcos.first()));

}
//-----------------------------------------------------------------------------------------------
void StartExchange::showWdgtByName(const QString &wdgtAccessibleName, const QString &wdgtTitle)
{
    showWdgtByName(wdgtAccessibleName, wdgtTitle, QIcon());

}
//-----------------------------------------------------------------------------------------------
void StartExchange::showWdgtByName(const QString &wdgtAccessibleName, const QString &wdgtTitle, const QIcon &itemIcon)
{
    if(wdgtAccessibleName != lastWdgtAccessibleName){
//        gHelper->closeYourPopupsSlot();
        lastWdgtAccessibleName = wdgtAccessibleName;
    }
    if(GuiHelper::stackContainsThisWdgt(ui->swDeviceOperations, wdgtAccessibleName, true, 0)){
        MatildaConfWidget *w = currentMatildaWidget();
        if(w){
            w->pageActivated();
            w->setupGlobalLblMessage(ui->lblPageMess);
        }
    }else{
        addWdgt2devStack(wdgtAccessibleName, wdgtTitle, itemIcon);
    }

}

//-----------------------------------------------------------------------------------------------
void StartExchange::on_tbIfaceSett_clicked()
{
    //open iface settings widget

    QStringList listIcos, chListNames;


    //, QStringList chListNames, QStringList listIcos, int buttonH)


    const QStringList chListData = getChListData(listIcos, chListNames);

    const int row = chListData.indexOf("Interface");
    if(row < 0)
        return;
//QString("Poll;Relay;Queue;Statistic of the exchange;Date and time;Meter address;Check Connection Tool;Other;Interface").split(";");

    showWdgtByName(chListData.at(row), chListNames.at(row), QIcon(listIcos.at(row)));
//ui->trDevOperation->model()
    TreeModel *modelDevOptTree { static_cast<TreeModel *>(ui->trDevOperation->model())};
    if(modelDevOptTree)
        ui->trDevOperation->setCurrentIndex(modelDevOptTree->index(row,0));



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

void StartExchange::on_trDevOperation_clicked(const QModelIndex &index)
{
    if(index.isValid()){
        if(true){
            MatildaConfWidget *w = currentMatildaWidget();
            if(w)
                w->disableGlobalLblMessage(ui->lblPageMess);
        }


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


    const QStringList chListData = getChListData(listIcos, chListNames);

//QString("Poll;Relay;Queue;Statistic of the exchange;Date and time;Meter address;Check Connection Tool;Other;Interface").split(";");
    MatildaConfWidget *w = 0;

    const int row = chListData.indexOf(realPageName);
    switch(row){
    case 0: w = new StartPagePoll( lDevInfo, gHelper, gSett4all, this); break;
    case 1: w = new RelayWdgt( lDevInfo, gHelper, gSett4all, this); break;

    case 2: w = new ZbyratorTasks( lDevInfo, gHelper, gSett4all, this); break;
    case 3: w = new StatisticOfExchangeWdgt( lDevInfo, gHelper, gSett4all, this); break;

    case 4: w = new MetersDateTime( lDevInfo, gHelper, gSett4all, this); break;
    case 5: w = new SetMeterAddress( lDevInfo, gHelper, gSett4all, this); break;

    case 6: w = new CheckConnectionToolWdgt( lDevInfo, gHelper, gSett4all, this); break;
    case 7: w = new ZbyratorService( lDevInfo, gHelper, gSett4all, this); break;

    case 8: w = new ZbyrIfaceSett( lDevInfo, gHelper, gSett4all, this); break;

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
        w->setRwCommand(1, 1);// MatildaDeviceTree::getPageCanRead().at(row), MatildaDeviceTree::getPageCanWrite().at(row));

//        w->setAccessibleName(realPageName);

        w->setupGlobalLblMessage(ui->lblPageMess);
        QWidget *sa = StackWidgetHelper::addWdgtWithScrollArea(this, w, realPageName);
        ui->swDeviceOperations->addWidget(sa);
        ui->swDeviceOperations->setCurrentWidget(sa);
    }

}

