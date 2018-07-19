#include "checkconnectiontoolwdgt.h"
#include "ui_checkconnectiontoolwdgt.h"

CheckConnectionToolWdgt::CheckConnectionToolWdgt(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    ReferenceWidgetClass(lDevInfo, gHelper, gSett4all, parent),
    ui(new Ui::CheckConnectionToolWdgt)
{
    ui->setupUi(this);
}

CheckConnectionToolWdgt::~CheckConnectionToolWdgt()
{
    delete ui;
}

void CheckConnectionToolWdgt::initPage()
{
    ui->pushButton->setEnabled(false);

    connect(gHelper, SIGNAL(setPbReadEnableDisable(bool)), ui->pushButton, SLOT(setDisabled(bool)));
    ui->pushButton->setDisabled(gHelper->managerEnDisBttn.pbReadDis);
}

void CheckConnectionToolWdgt::on_pushButton_clicked()
{

}
