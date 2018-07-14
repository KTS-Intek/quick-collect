#include "startpagepoll.h"
#include "ui_startpagepoll.h"

StartPagePoll::StartPagePoll(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    MatildaConfWidget(lDevInfo, gHelper, gSett4all, parent),
    ui(new Ui::StartPagePoll)
{
    ui->setupUi(this);
}

StartPagePoll::~StartPagePoll()
{
    delete ui;
}

void StartPagePoll::on_pbReadDb_clicked()
{

}
