#include "waterstartpollsleepdlg.h"
#include "ui_waterstartpollsleepdlg.h"

WaterStartPollSleepDlg::WaterStartPollSleepDlg(const WtrSleepPollSett &sett, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WaterStartPollSleepDlg)
{
    ui->setupUi(this);
    setWindowTitle(tr("Power management"));
    ui->cbxCheckSleepProfile->setChecked(sett.checkProfile);
    ui->sbSleepSecsAfter->setValue(sett.secs);
    ui->cbxPwrManagement->setChecked(sett.sendSleepCommand);
    on_cbxPwrManagement_clicked(ui->cbxPwrManagement->isChecked());
}

WaterStartPollSleepDlg::~WaterStartPollSleepDlg()
{
    delete ui;
}

void WaterStartPollSleepDlg::on_buttonBox_accepted()
{
    emit setWaterSleepSett(ui->cbxPwrManagement->isChecked(), ui->sbSleepSecsAfter->value(), ui->cbxCheckSleepProfile->isChecked());
}

void WaterStartPollSleepDlg::on_cbxPwrManagement_clicked(bool checked)
{
    ui->sbSleepSecsAfter->setEnabled(checked);
}
