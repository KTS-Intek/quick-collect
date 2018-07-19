#include "setmeteraddress.h"
#include "ui_setmeteraddress.h"

SetMeterAddress::SetMeterAddress(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    MatildaConfWidget(lDevInfo, gHelper, gSett4all, parent),
    ui(new Ui::SetMeterAddress)
{
    ui->setupUi(this);
}

SetMeterAddress::~SetMeterAddress()
{
    delete ui;
}

void SetMeterAddress::initPage()
{
    ui->pushButton->setEnabled(false);

    connect(gHelper, SIGNAL(setPbReadEnableDisable(bool)), ui->pushButton, SLOT(setDisabled(bool)));
    ui->pushButton->setDisabled(gHelper->managerEnDisBttn.pbReadDis);
}

void SetMeterAddress::on_pushButton_clicked()
{

}
