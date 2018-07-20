#include "selectmeters4poll.h"
#include "ui_selectmeters4poll.h"
#include "src/widgets/selectionchecker.h"
#include "src/matilda/settloader.h"
#include "src/zbyrator-v2/quickpollhelper.h"
#include "zbyrator-src/src/getreadymetersdata.h"

SelectMeters4poll::SelectMeters4poll(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    ReferenceWidgetClass(lDevInfo, gHelper, gSett4all, parent),
    ui(new Ui::SelectMeters4poll)
{
    ui->setupUi(this);
    connect(ui->pbCancel, SIGNAL(clicked(bool)), this, SLOT(deleteLater()) );
}

SelectMeters4poll::~SelectMeters4poll()
{
    delete ui;
}

void SelectMeters4poll::setPollSett(QDateTime dtFrom, QDateTime dtTo, quint8 pollCode)
{
    //start select data
    lPollSett.dtFrom = dtFrom;
    lPollSett.dtTo = dtTo;
    lPollSett.pollCode = pollCode;

    //



}

void SelectMeters4poll::initPage()
{
    setupObjects(ui->tvTable, ui->tbFilter, ui->cbFilterMode, ui->leFilter, SETT_FILTERS_ZBYR_SELMTRS);
    StandardItemModelHelper::modelSetHorizontalHeaderItems(model, QStringList());


    SelectionChecker *tmr = new SelectionChecker(this);
    tmr->setWatchTable(ui->tvTable, ui->pbSelected);
    setHasDataFromRemoteDevice();

    GetReadyMetersData *d = new GetReadyMetersData;
    QThread *t = new QThread;
    d->setPollSett(lPollSett.dtFrom, lPollSett.dtTo, lPollSett.pollCode);

    d->moveToThread(t);

    connect(ui->pbCancel, SIGNAL(clicked(bool)), d, SLOT(stopAllDirect()), Qt::DirectConnection);
    connect(this, SIGNAL(destroyed(QObject*)), d, SLOT(stopAllDirect()), Qt::DirectConnection);
    connect(d, SIGNAL(destroyed(QObject*)), t, SLOT(quit()));
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()) );

    connect(t, SIGNAL(started()), d, SLOT(onThreadStarted()) );
    connect(t, SIGNAL(started()), this, SIGNAL(onReloadAllMeters()) );

    connect(d, SIGNAL(allMeters2selectWdgt(MyListStringList,QVariantMap,QStringList,QStringList,bool)), this, SLOT(setPageSett(MyListStringList,QVariantMap,QStringList,QStringList,bool)));
    connect(d, SIGNAL(onProcessingEnds(QStringList)), this, SLOT(onProcessingEnds(QStringList)));

    connect(this, SIGNAL(onAllMeters(UniversalMeterSettList)), d, SLOT(setMetersList(UniversalMeterSettList)));

    t->start();

//    emit onPageCanReceiveData();
}

void SelectMeters4poll::setPageSett(const MyListStringList &listRows, const QVariantMap &col2data, const QStringList &headerH, const QStringList &header, const bool &hasHeader)
{

//    const QString currNi = headerH.contains("NI") ? TableViewHelper::getCellValueOfcurrentRow(ui->tvTable, headerH.indexOf("NI")) : "";

    StandardItemModelHelper::append2model(listRows, col2data, headerH, header, hasHeader, model);


//    if(!currNi.isEmpty())
//        TableViewHelper::selectRowWithThisCell(ui->tvTable, currNi, headerH.indexOf("NI"));

    const bool enableBnts = !listRows.isEmpty();

    ui->pbCheckedOn->setEnabled(enableBnts);
    ui->pbPollAll->setEnabled(enableBnts);


    emit resizeTv2content(ui->tvTable);
}

void SelectMeters4poll::onProcessingEnds(QStringList listMissingData)
{//    //tr("Meter;NI;Memo;Poll;Has data").split(";"));
    ui->lblDatabase->hide();
    for(int i = 0, imax = model->rowCount(); i < imax; i++){
         if(listMissingData.contains(model->item(i,1)->text())){
            listMissingData.removeOne(model->item(i,1)->text());
            model->item(i, 3)->setText("+");
            model->item(i, 4)->setText("!");
        }else{
             model->item(i, 3)->setText("-");
             model->item(i, 4)->setText(tr("all"));
        }
    }
}

void SelectMeters4poll::on_pbPollAll_clicked()
{
    QStringList l;
    for(int i = 0, imax = proxy_model->rowCount(); i < imax; i++){
        const int row = proxy_model->mapToSource(proxy_model->index(i, 0)).row();
        if(row < 0)
            continue;

        l.append(model->item(row, 1)->text());
    }
    sendStartPoll(l);
}

void SelectMeters4poll::sendStartPoll(const QStringList &listni)
{
    if(listni.isEmpty())
        return;

    QString mess;
    const QString args = QuickPollHelper::createQuickPollLine(listni.join(" "), "", "", "", -1, lPollSett.dtTo, lPollSett.dtFrom, mess);

    if(args.isEmpty())
        return;

    emit command4dev(lPollSett.pollCode, args);
    QTimer::singleShot(11, this, SLOT(deleteLater()) );
}

void SelectMeters4poll::on_pbSelected_clicked()
{
    sendStartPoll(TableViewHelper::selectedRowText(ui->tvTable, 1));
}

void SelectMeters4poll::on_pbCheckedOn_clicked()
{
    QStringList l;
    for(int i = 0, imax = proxy_model->rowCount(); i < imax; i++){
        const int row = proxy_model->mapToSource(proxy_model->index(i, 0)).row();
        if(row < 0)
            continue;

        if(model->item(row, 3)->text() == "+")
            l.append(model->item(row, 1)->text());
    }
    sendStartPoll(l);
}
