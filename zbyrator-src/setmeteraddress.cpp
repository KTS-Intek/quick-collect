#include "setmeteraddress.h"
#include "ui_setmeteraddress.h"

SetMeterAddress::SetMeterAddress(GuiHelper *gHelper, QWidget *parent) :
    MatildaConfWidget(gHelper,  parent),
    ui(new Ui::SetMeterAddress)
{
    ui->setupUi(this);
    ui->pushButton->setEnabled(false);
}

SetMeterAddress::~SetMeterAddress()
{
    delete ui;
}

void SetMeterAddress::initPage()
{
    ui->pushButton->setEnabled(false);

    connect(this, SIGNAL(lockButtons(bool)), ui->pushButton, SLOT(setDisabled(bool)));
}

void SetMeterAddress::on_pushButton_clicked()
{

}
