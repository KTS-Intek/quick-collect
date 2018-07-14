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

void ZbyratorService::on_pbOpenDa_clicked()
{

}
