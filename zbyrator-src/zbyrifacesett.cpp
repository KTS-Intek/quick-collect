#include "zbyrifacesett.h"
#include "ui_zbyrifacesett.h"

ZbyrIfaceSett::ZbyrIfaceSett(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    MatildaConfWidget(lDevInfo, gHelper, gSett4all, parent),
  ui(new Ui::ZbyrIfaceSett)
{
    ui->setupUi(this);
}

ZbyrIfaceSett::~ZbyrIfaceSett()
{
    delete ui;
}

void ZbyrIfaceSett::on_tbUpdateSerial_clicked()
{

}
