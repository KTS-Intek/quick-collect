#include "watersleepscheduler.h"
#include "ui_watersleepscheduler.h"

#include "src/matilda/settloader.h"

//відображення зчитаного розкладу з лічильників
//запис розкладу до лічильників

WaterSleepScheduler::WaterSleepScheduler(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    ReferenceWidgetClass(lDevInfo, gHelper, gSett4all, parent),
    ui(new Ui::WaterSleepScheduler)
{
    ui->setupUi(this);
}

WaterSleepScheduler::~WaterSleepScheduler()
{
    delete ui;
}

QVariantHash WaterSleepScheduler::getPageSett(bool &ok, QString &mess, const bool &is4copy)
{

    return QVariantHash();
}

void WaterSleepScheduler::initPage()
{

    setupObjects(ui->tvTable, ui->tbFilter, ui->cbFilterMode, ui->leFilter, SETT_FILTERS_ZBYR_WTR_SCHDL);

    emit onReloadAllMeters();

}

void WaterSleepScheduler::clearPage()
{

}

void WaterSleepScheduler::setPageSett(const MyListStringList &listRows, const QVariantMap &col2data, const QStringList &headerH, const QStringList &header, const bool &hasHeader)
{//

}

void WaterSleepScheduler::on_tbAdd_clicked()
{

}
