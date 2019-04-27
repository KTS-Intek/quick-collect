#include "ucemulator.h"
#include "ui_ucemulator.h"

UcEmulator::UcEmulator(GuiHelper *gHelper, QWidget *parent) :
    MatildaConfWidget(gHelper,  parent),
    ui(new Ui::UcEmulator)
{
    ui->setupUi(this);
}

UcEmulator::~UcEmulator()
{
    delete ui;
}
