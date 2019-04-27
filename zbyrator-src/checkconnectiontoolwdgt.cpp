#include "checkconnectiontoolwdgt.h"
#include "ui_checkconnectiontoolwdgt.h"

CheckConnectionToolWdgt::CheckConnectionToolWdgt(GuiHelper *gHelper, QWidget *parent) :
    ReferenceWidgetClass(gHelper,  parent),
    ui(new Ui::CheckConnectionToolWdgt)
{
    ui->setupUi(this);
    ui->pushButton->setEnabled(false);
}

CheckConnectionToolWdgt::~CheckConnectionToolWdgt()
{
    delete ui;
}

void CheckConnectionToolWdgt::initPage()
{
    ui->pushButton->setEnabled(false);

    connect(this, SIGNAL(lockButtons(bool)), ui->pushButton, SLOT(setDisabled(bool)));
}

void CheckConnectionToolWdgt::on_pushButton_clicked()
{

}
