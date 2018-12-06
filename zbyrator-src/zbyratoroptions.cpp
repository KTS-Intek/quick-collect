#include "zbyratoroptions.h"
#include "ui_zbyratoroptions.h"
#include "gui-src/settloader.h"
#include "gui-src/stackwidgethelper.h"

#include "zbyrator-src/options/databasesettings.h"
#include "dataconcetrator-pgs/pollwdgt.h"
#include "zbyrator-src/options-src/pollwdgtmom.h"
#include "zbyrator-src/options/ucemulator.h"
#include "zbyrator-src/options/ktsconnectmode.h"
#include "main-pgs/optionswdgt.h"


ZbyratorOptions::ZbyratorOptions(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    ReferenceWidgetClass(lDevInfo, gHelper, gSett4all, parent),
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
    setupObjects(0, 0, 0, ui->leFilter, SETT_FILTERS_ZBYROPTSLV);
    StandardItemModelHelper::modelSetHorizontalHeaderItems(model, tr("Pages").split("\n"));


    addItems2model();

}

void ZbyratorOptions::onLvOptions_clicked(const QModelIndex &index)
{
    const int row = proxy_model->mapToSource(index).row();
    if(row < 0)
        return;


    if(!GuiHelper::stackContainsThisWdgt(ui->stackedWidget, QString("options_%1").arg(row), true))
        addThisWdgt2stack(row, model->item(row)->text(), QString("options_%1").arg(row));


}

void ZbyratorOptions::addItems2model()
{
    ui->lvOptions->setModel(proxy_model);
    QStringList list;
    list.append(tr("Local database"));
    list.append(tr("Poll settings"));
    list.append(tr("UC Emulator"));
    list.append(tr("KTS Connect"));
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
    case 0: w = new DatabaseSettings(lDevInfo, gHelper, gSett4all, this); break;
    case 1: w = createPollWdgt(); break;
    case 2: w = new UcEmulator(lDevInfo, gHelper, gSett4all, this); break;
    case 3: w = new KtsConnectMode(lDevInfo, gHelper, gSett4all, this); break;
    case 4: w = new OptionsWdgt(lDevInfo, gHelper, gSett4all, this); break;
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

        w->setRwCommand(1, 0xFFFE);
        w->setHasDataFromRemoteDevice();
//        w->setupGlobalLblMessage(ui->lblPageMess);
        StackWidgetHelper::addWdgtWithScrollArea2stackedWdgt(this, ui->stackedWidget, w, realPageName);
    }
}

MatildaConfWidget *ZbyratorOptions::createPollWdgt()
{

//    PollWdgtMom *m = ;
    return (new PollWdgtMom(new PollWdgt(lDevInfo, gHelper, gSett4all, this), this))->pollWdgt;
}

void ZbyratorOptions::on_stackedWidget_currentChanged(int arg1)
{

}
