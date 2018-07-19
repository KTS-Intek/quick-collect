#include "zbyratorservice.h"
#include "ui_zbyratorservice.h"

ZbyratorService::ZbyratorService(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
   MatildaConfWidget(lDevInfo, gHelper, gSett4all, parent),
    ui(new Ui::ZbyratorService)
{
    ui->setupUi(this);
}

ZbyratorService::~ZbyratorService()
{
    delete ui;
}

void ZbyratorService::initPage()
{

    connect(gHelper, SIGNAL(setPbReadEnableDisable(bool)), ui->pbOpenDa, SLOT(setDisabled(bool)));
    connect(gHelper, SIGNAL(setPbReadEnableDisable(bool)), ui->pbResetNetwork, SLOT(setDisabled(bool)));

    ui->pbOpenDa->setDisabled(gHelper->managerEnDisBttn.pbReadDis);
    ui->pbResetNetwork->setDisabled(gHelper->managerEnDisBttn.pbReadDis);

}

void ZbyratorService::on_pbOpenDa_clicked()
{

}
