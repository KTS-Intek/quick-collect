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

    connect(gHelper, SIGNAL(setPbWriteEnableDisable(bool)), ui->pbOpenDa, SLOT(setDisabled(bool)));
    connect(gHelper, SIGNAL(setPbWriteEnableDisable(bool)), ui->pbResetNetwork, SLOT(setDisabled(bool)));

    ui->pbOpenDa->setDisabled(gHelper->managerEnDisBttn.pbWriteDis);
    ui->pbResetNetwork->setDisabled(gHelper->managerEnDisBttn.pbWriteDis);

}

void ZbyratorService::on_pbOpenDa_clicked()
{

}
