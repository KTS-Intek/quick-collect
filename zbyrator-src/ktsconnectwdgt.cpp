#include "ktsconnectwdgt.h"
#include "ui_ktsconnectwdgt.h"

KtsConnectWdgt::KtsConnectWdgt(GuiHelper *gHelper, QWidget *parent) :
    MatildaConfWidget(gHelper, parent),
    ui(new Ui::KtsConnectWdgt)
{
    ui->setupUi(this);
}

KtsConnectWdgt::~KtsConnectWdgt()
{
    delete ui;
}
