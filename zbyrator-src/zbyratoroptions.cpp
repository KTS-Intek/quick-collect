#include "zbyratoroptions.h"
#include "ui_zbyratoroptions.h"
#include "src/nongui/settloader.h"
#include "gui-src/stackwidgethelper.h"

#include "zbyrator-src/options/databasesettings.h"
#include "dataconcetrator-pgs/pollwdgt.h"
#include "zbyrator-src/options-src/pollwdgtmom.h"
#include "zbyrator-src/options/ucemulator.h"
#include "zbyrator-src/options/ktsconnectmode.h"
#include "main-pgs/optionswdgt.h"


ZbyratorOptions::ZbyratorOptions(GuiHelper *gHelper, QWidget *parent) :
    ReferenceWidgetClass(gHelper,  parent),
    ui(new Ui::ZbyratorOptions)
{
    ui->setupUi(this);
}

ZbyratorOptions::~ZbyratorOptions()
{
    delete ui;
}

void ZbyratorOptions::initPage()
{
    setupObjects(0, 0, 0, 0, ui->leFilter, SETT_FILTERS_ZBYROPTSLV);
    StandardItemModelHelper::setModelHorizontalHeaderItems(model, tr("Pages").split("\n"));


    addItems2model();

    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(deleteLater()));
}

void ZbyratorOptions::onLvOptions_clicked(const QModelIndex &index)
{
    const int row = proxy_model->mapToSource(index).row();
    if(row < 0)
        return;


    if(!WidgetsHelper::stackContainsThisWdgt(ui->stackedWidget, QString("options_%1").arg(row), true))
        addThisWdgt2stack(row, model->item(row)->text(), QString("options_%1").arg(row));


}

void ZbyratorOptions::addItems2model()
{
    ui->lvOptions->setModel(proxy_model);
    QStringList list;
    list.append(tr("Local database"));
    list.append(tr("Poll settings"));
    list.append(tr("UC Emulator"));
//    list.append(tr("KTS Connect"));
    list.append(tr("Application"));

    const QSize sh = QSize(ui->leFilter->width(), ui->leFilter->height() * 1.2);

    while(!list.isEmpty()){
        QStandardItem *item = new QStandardItem(list.takeFirst());
        item->setSizeHint(sh);
        model->appendRow(item);
    }
    connect(ui->lvOptions, SIGNAL(clicked(QModelIndex)), this, SLOT(onLvOptions_clicked(QModelIndex)));

    ui->leFilter->clear();

    ui->lvOptions->setCurrentIndex(proxy_model->index(0,0));

    onLvOptions_clicked(ui->lvOptions->currentIndex());
}

void ZbyratorOptions::addThisWdgt2stack(const int &row, const QString &pageName, const QString &realPageName)
{
    MatildaConfWidget *w = 0;
    switch(row){
    case 0:{

        DatabaseSettings *dw = new DatabaseSettings(gHelper,  this);

        connect(dw, &DatabaseSettings::command2extension, this, &ZbyratorOptions::command2extensionBBB);

        w = dw;
        break;}

    case 1: w = createPollWdgt(); break;
    case 2:{

//        GuiHelper *gHelper = new GuiHelper(this);
//        gHelper->guiSett = this->gHelper->guiSett;
//         gHelper->initObj();
//         gHelper->parentWidget = ui->stackedWidget;
//        gHelper->lDevInfo = this->gHelper->lDevInfo;

        UcEmulator *e = new UcEmulator(gHelper->guiSett->enableTestFeatures, gHelper->guiSett, gHelper->ucDeviceTreeW,  this);
        connect(e, &UcEmulator::reloadSettings2ucEmulator, this, &ZbyratorOptions::reloadSettings2ucEmulator);
        connect(e, &UcEmulator::command2extension           , this, &ZbyratorOptions::command2extension);
        e->setWindowTitle(pageName);
        StackWidgetHelper::addWdgtWithScrollArea2stackedWdgt(this, ui->stackedWidget, e, realPageName);

//        connect(e, SIGNAL(startDaServer(qint8,quint16))      , this, SIGNAL(startDaServer(qint8,quint16))   );
//        connect(e, SIGNAL(stopDaServer())                    , this, SIGNAL(stopDaServer())                 );


        return;}
//    case 3: w = new KtsConnectMode(gHelper,  this); break;
    case 3: w = createOptionsWdgt(); break;
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
//        const TabName2icosPaths ttn = CreateToolBar::getTabs().at(row);

        w->setWindowTitle(pageName);
//        w->setWindowIcon(QIcon(ttn.path));


//        connect(this, SIGNAL(killMyChild()), w, SLOT(deleteLater()) );
//        connect(this, SIGNAL(onOperationNError(int)), w, SLOT(onOperationNError(int)) );
//        w->setRwCommand(MatildaDeviceTree::getPageCanRead().at(row), MatildaDeviceTree::getPageCanWrite().at(row));

//        w->setRwCommand(1, 0xFFFE);
//        w->setHasDataFromRemoteDevice();
//        w->setupGlobalLblMessage(ui->lblPageMess);
        StackWidgetHelper::addWdgtWithScrollArea2stackedWdgt(this, ui->stackedWidget, w, realPageName);
    }
}

MatildaConfWidget *ZbyratorOptions::createPollWdgt()
{

//    PollWdgtMom *m = ;
    return (new PollWdgtMom(new PollWdgt(gHelper,  this), this))->pollWdgt;
}

MatildaConfWidget *ZbyratorOptions::createOptionsWdgt()
{
    OptionsWdgt *w = new OptionsWdgt(gHelper, this);
//    w->hideButtons();
    QPushButton *p = w->getPbClose();
    p->setText(tr("Reload"));
    p->setIcon(QIcon(":/katynko/svg/view-refresh.svg"));
    disconnect(p, SIGNAL(clicked(bool)), w, SLOT(deleteLater()));
    connect(p, SIGNAL(clicked(bool)), w, SLOT(loadSettPageOptions()));

    connect(w, SIGNAL(destroyed(QObject*)), gHelper, SLOT(updateSettDateMaskAndDotPos()));

    return w;
}

//void ZbyratorOptions::on_stackedWidget_currentChanged(int arg1)
//{

//}

