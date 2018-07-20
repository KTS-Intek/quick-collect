#include "ktsconnectmode.h"
#include "ui_ktsconnectmode.h"

KtsConnectMode::KtsConnectMode(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    ReferenceWidgetClass(lDevInfo, gHelper, gSett4all, parent),
    ui(new Ui::KtsConnectMode)
{
    ui->setupUi(this);
}

KtsConnectMode::~KtsConnectMode()
{
    delete ui;
}
