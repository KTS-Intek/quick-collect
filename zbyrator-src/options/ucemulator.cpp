#include "ucemulator.h"
#include "ui_ucemulator.h"

UcEmulator::UcEmulator(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    MatildaConfWidget(lDevInfo, gHelper, gSett4all, parent),
    ui(new Ui::UcEmulator)
{
    ui->setupUi(this);
}

UcEmulator::~UcEmulator()
{
    delete ui;
}
