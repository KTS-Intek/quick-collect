#include "metersdatetimebuttonswdgt.h"
#include "ui_metersdatetimebuttonswdgt.h"

///[!] widgets-shared
#include "src/widgets/selectionchecker.h"


#include "definedpollcodes.h"

MetersDateTimeButtonsWdgt::MetersDateTimeButtonsWdgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MetersDateTimeButtonsWdgt)
{
    ui->setupUi(this);
    setButtonsEnable(false);
}

MetersDateTimeButtonsWdgt::~MetersDateTimeButtonsWdgt()
{
    delete ui;
}

QList<QPushButton *> MetersDateTimeButtonsWdgt::gimmeYourButtons()
{
    QList<QPushButton *> la;

    la.append(ui->pbCorrectionAll);
    la.append(ui->pbRead);

    la.append(ui->pbWrite);



    return la;
}

bool MetersDateTimeButtonsWdgt::gimmeYourEnabled()
{
    return (ui->pbRead->isEnabled() && ui->widget_2->isEnabled());

}

void MetersDateTimeButtonsWdgt::createSelectionChecker(QTableView *lastTv)
{
    SelectionChecker *tmr = new SelectionChecker(this);
    tmr->setWatchTable(lastTv, ui->widget_2);
    tmr->setTextLbl4disp(ui->label_2, tr("Selected [%1]"), tr("Selected"));

    connect(tmr, &SelectionChecker::setWdgtDisable, this, &MetersDateTimeButtonsWdgt::onWdgtLock);// ui->widget_3, &QWidget::setVisible);
}

void MetersDateTimeButtonsWdgt::setButtonsEnable(bool enable)
{

    ui->pbWrite->setEnabled(enable);
    ui->pbCorrectionAll->setEnabled(enable);
    ui->pbRead->setEnabled(enable);
}

void MetersDateTimeButtonsWdgt::lockButtons(bool disable)
{
    setButtonsEnable(!disable);

}

void MetersDateTimeButtonsWdgt::onWdgtLock(bool disable)
{
    sendActLock(disable, !ui->pbRead->isEnabled());

}

void MetersDateTimeButtonsWdgt::sendActLock(const bool &isWdgtDisabled, const bool &isButtonDisabled)
{
    emit lockActions((isWdgtDisabled || isButtonDisabled));

}

void MetersDateTimeButtonsWdgt::on_pbCorrectionAll_clicked()
{
    startOperation(false, POLL_CODE_WRITE_DATE_TIME);
}

void MetersDateTimeButtonsWdgt::on_pbRead_clicked()
{
    startOperation(true, POLL_CODE_READ_DATE_TIME_DST);
}

void MetersDateTimeButtonsWdgt::on_pbWrite_clicked()
{
    startOperation(true, POLL_CODE_WRITE_DATE_TIME);

}

void MetersDateTimeButtonsWdgt::startOperation(const bool &selectedOnly, const quint8 &operation)
{
    emit onStartOperation(selectedOnly, operation);
}
