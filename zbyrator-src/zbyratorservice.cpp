#include "zbyratorservice.h"
#include "ui_zbyratorservice.h"

#include "definedpollcodes.h"



///[!] tasks-shared
#include "src/shared/embeemodemglobalparams.h"


ZbyratorService::ZbyratorService(GuiHelper *gHelper, QWidget *parent) :
   MatildaConfWidget(gHelper,  parent),
    ui(new Ui::ZbyratorService)
{
    ui->setupUi(this);

    ui->pbResetNetwork->setEnabled(false);
    ui->pbDisableApi->setEnabled(false);
    ui->pbEnableApi->setEnabled(false);
    ui->pbModemReset->setEnabled(false);

}

ZbyratorService::~ZbyratorService()
{
    delete ui;
}

QStringList ZbyratorService::getWriteCommands(const bool &enableapi)
{
    QStringList list;
    list.append("ATAD C");
    list.append("ATSM0");
    list.append(QString("ATAP%1").arg(int(enableapi)));
    list.append("ATWR");
    return list;
}

void ZbyratorService::initPage()
{

    connect(this, SIGNAL(lockButtons(bool)), ui->pbDisableApi, SLOT(setDisabled(bool)));
    connect(this, SIGNAL(lockButtons(bool)), ui->pbEnableApi, SLOT(setDisabled(bool)));
    connect(this, SIGNAL(lockButtons(bool)), ui->pbModemReset, SLOT(setDisabled(bool)));
    connect(this, SIGNAL(lockButtons(bool)), ui->pbResetNetwork, SLOT(setDisabled(bool)));

}



void ZbyratorService::on_pbResetNetwork_clicked()
{
    startPollCode(POLL_CODE_RESET_NETWORK);



}

void ZbyratorService::startPollCode(const quint8 &pollCode)
{
    const QVariantMap map = tryStartTheOperation(pollCode);
    if(map.isEmpty())
        return ;
    gHelper->updateSettDateMaskAndDotPos();

    emit command4dev(pollCode, map);

}

QVariantMap ZbyratorService::tryStartTheOperation(int pollCode)
{
    const QStringList ns = (pollCode < POLL_CODE_MODEM_OPEREATIONS_END) ? EmbeeModemGlobalParams::getNi4taskList(pollCode) : QStringList();

    if(ns.isEmpty())
        return QVariantMap();


    QVariantMap map;
    map.insert("-ns", ns);

    switch(pollCode){
    case POLL_CODE_WRITE_MODEM_CONFIG:{
        map.insert("-operation", getWriteCommands(lastWasEnableApi).join("\n"));
        break;}
    }

    return map;

}


void ZbyratorService::on_pbModemReset_clicked()
{
    startPollCode(POLL_CODE_RESET_MODEM);

}

void ZbyratorService::on_pbEnableApi_clicked()
{
    lastWasEnableApi = true;
    startPollCode(POLL_CODE_WRITE_MODEM_CONFIG);

}

void ZbyratorService::on_pbDisableApi_clicked()
{
    lastWasEnableApi = false;
    startPollCode(POLL_CODE_WRITE_MODEM_CONFIG);
}
