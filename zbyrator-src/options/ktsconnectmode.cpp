#include "ktsconnectmode.h"
#include "ui_ktsconnectmode.h"

KtsConnectMode::KtsConnectMode(GuiHelper *gHelper, QWidget *parent) :
    ReferenceWidgetClass(gHelper,  parent),
    ui(new Ui::KtsConnectMode)
{
    ui->setupUi(this);
}

KtsConnectMode::~KtsConnectMode()
{
    delete ui;
}
