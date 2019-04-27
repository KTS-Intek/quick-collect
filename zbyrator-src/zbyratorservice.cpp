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
    ui->pbOpenDa->setEnabled(false);
    ui->pbResetNetwork->setEnabled(false);
}

ZbyratorService::~ZbyratorService()
{
    delete ui;
}

void ZbyratorService::initPage()
{

    connect(this, SIGNAL(lockButtons(bool)), ui->pbOpenDa, SLOT(setDisabled(bool)));
    connect(this, SIGNAL(lockButtons(bool)), ui->pbResetNetwork, SLOT(setDisabled(bool)));

}

void ZbyratorService::on_pbOpenDa_clicked()
{

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
    return map;

}
