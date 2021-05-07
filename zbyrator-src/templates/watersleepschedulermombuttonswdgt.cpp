#include "watersleepschedulermombuttonswdgt.h"
#include "ui_watersleepschedulermombuttonswdgt.h"

///[!] widgets-shared
#include "src/widgets/selectionchecker.h"


#include "definedpollcodes.h"



WaterSleepSchedulerMomButtonsWdgt::WaterSleepSchedulerMomButtonsWdgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WaterSleepSchedulerMomButtonsWdgt)
{
    ui->setupUi(this);
    setButtonsEnable(false);

}

WaterSleepSchedulerMomButtonsWdgt::~WaterSleepSchedulerMomButtonsWdgt()
{
    delete ui;
}

void WaterSleepSchedulerMomButtonsWdgt::createSelectionChecker(QTableView *lastTv)
{
    SelectionChecker *tmr = new SelectionChecker(this);
    tmr->setWatchTable(lastTv, ui->widget_2);
    tmr->setTextLbl4disp(ui->label_2, tr("Selected [%1]"), tr("Selected"));

    connect(tmr, &SelectionChecker::setWdgtDisable, this, &WaterSleepSchedulerMomButtonsWdgt::onWdgtLock);// ui->widget_3, &QWidget::setVisible);
}

void WaterSleepSchedulerMomButtonsWdgt::setButtonsEnable(bool enable)
{
    ui->pbRead->setEnabled(enable);
    ui->pbWrite->setEnabled(enable);

}

void WaterSleepSchedulerMomButtonsWdgt::lockButtons(bool disable)
{
    setButtonsEnable(!disable);

}

void WaterSleepSchedulerMomButtonsWdgt::onWdgtLock(bool disable)
{
    sendActLock(disable, !ui->pbRead->isEnabled());

}

void WaterSleepSchedulerMomButtonsWdgt::sendActLock(const bool &isWdgtDisabled, const bool &isButtonDisabled)
{
    emit lockActions((isWdgtDisabled || isButtonDisabled));

}

void WaterSleepSchedulerMomButtonsWdgt::on_pbRead_clicked()
{
    doOperationSelected(POLL_CODE_READ_WTR_SLEEP);

}

void WaterSleepSchedulerMomButtonsWdgt::on_pbWrite_clicked()
{
    doOperationSelected(POLL_CODE_WRITE_WTR_SLEEP);

}

void WaterSleepSchedulerMomButtonsWdgt::doOperationSelected(const quint8 &operation)
{
    emit onDoOperationSelected(operation);

}
