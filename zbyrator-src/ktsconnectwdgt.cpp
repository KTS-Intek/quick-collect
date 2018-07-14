#include "ktsconnectwdgt.h"
#include "ui_ktsconnectwdgt.h"

KtsConnectWdgt::KtsConnectWdgt(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    MatildaConfWidget(lDevInfo, gHelper, gSett4all, parent),
    ui(new Ui::KtsConnectWdgt)
{
    ui->setupUi(this);
}

KtsConnectWdgt::~KtsConnectWdgt()
{
    delete ui;
}
