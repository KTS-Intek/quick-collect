#include "selectdialogform.h"
#include "ui_selectdialogform.h"

SelectDialogForm::SelectDialogForm(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    QDialog(parent), lDevInfo(lDevInfo), gHelper(gHelper), gSett4all(gSett4all),
    ui(new Ui::SelectDialogForm)
{
    ui->setupUi(this);
    QTimer::singleShot(66, this, SLOT(createSelectWdgt()) );
}

SelectDialogForm::~SelectDialogForm()
{
    delete ui;
}

void SelectDialogForm::setPollSettSlot(QDateTime dtFrom, QDateTime dtTo, quint8 pollCode)
{
    lPollSett.dtFrom = dtFrom;
    lPollSett.dtTo = dtTo;
    lPollSett.pollCode = pollCode;
    resetPollSett();
}

void SelectDialogForm::resetPollSett()
{
    emit setPollSett(lPollSett.dtFrom, lPollSett.dtTo, lPollSett.pollCode);
}

void SelectDialogForm::createSelectWdgt()
{
    SelectMeters4poll *w = new SelectMeters4poll(lDevInfo, gHelper, gSett4all, this);
    connect(w, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()) );
    connect(w, SIGNAL(onPageCanReceiveData()), this, SLOT(resetPollSett()) );

    connect(this, SIGNAL(setPollSett(QDateTime,QDateTime,quint8)), w, SLOT(setPollSett(QDateTime,QDateTime,quint8)));

    ui->verticalLayout->addWidget(w);
}

