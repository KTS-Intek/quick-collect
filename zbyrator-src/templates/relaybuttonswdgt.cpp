#include "relaybuttonswdgt.h"
#include "ui_relaybuttonswdgt.h"

///[!] widgets-shared
#include "src/widgets/selectionchecker.h"


#include "definedpollcodes.h"

RelayButtonsWdgt::RelayButtonsWdgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RelayButtonsWdgt)
{
    ui->setupUi(this);

    setButtonsEnable(false);
}

RelayButtonsWdgt::~RelayButtonsWdgt()
{
    delete ui;
}

QList<QPushButton *> RelayButtonsWdgt::gimmeYourButtons()
{
    QList<QPushButton *> la;
    la.append(ui->pbRead);

    la.append(ui->pbLoadOff);
    la.append(ui->pbLoadOff_2);

    la.append(ui->pbLoadOn);
    la.append(ui->pbLoadOn_2);

    return la;
}

bool RelayButtonsWdgt::gimmeYourEnabled()
{
    return (ui->pbRead->isEnabled() && ui->widget_2->isEnabled());
}

void RelayButtonsWdgt::createSelectionChecker(QTableView *lastTv)
{

    SelectionChecker *tmr = new SelectionChecker(this);
    tmr->setWatchTable(lastTv, ui->widget_2);
    tmr->setTextLbl4disp(ui->label_2, tr("Selected [%1]"), tr("Selected"));

    connect(tmr, &SelectionChecker::setWdgtDisable, this, &RelayButtonsWdgt::onWdgtLock);// ui->widget_3, &QWidget::setVisible);

}

void RelayButtonsWdgt::setButtonsEnable(bool enable)
{
    ui->pbLoadOff->setEnabled(enable);
    ui->pbLoadOn->setEnabled(enable);
    ui->pbLoadOff_2->setEnabled(enable);
    ui->pbLoadOn_2->setEnabled(enable);
    ui->pbRead->setEnabled(enable);
}

void RelayButtonsWdgt::lockButtons(bool disable)
{
    setButtonsEnable(!disable);
}

void RelayButtonsWdgt::onWdgtLock(bool disable)
{
    sendActLock(disable, !ui->pbRead->isEnabled());

}

void RelayButtonsWdgt::sendActLock(const bool &isWdgtDisabled, const bool &isButtonDisabled)
{
    emit lockActions((isWdgtDisabled || isButtonDisabled));

}


void RelayButtonsWdgt::on_pbRead_clicked()
{
    doRelayOperationSelected(RELAY_READ);
}

void RelayButtonsWdgt::on_pbLoadOn_clicked()
{
    doRelayOperationSelected(RELAY_LOAD_ON);

}

void RelayButtonsWdgt::on_pbLoadOff_clicked()
{
    doRelayOperationSelected(RELAY_LOAD_OFF);
}



void RelayButtonsWdgt::on_pbLoadOff_2_clicked()
{
    doRelayOperationSelected(RELAY2_LOAD_OFF);

}

void RelayButtonsWdgt::on_pbLoadOn_2_clicked()
{
    doRelayOperationSelected(RELAY2_LOAD_ON);

}

void RelayButtonsWdgt::doRelayOperationSelected(const quint8 &operation)
{
    emit onDoRelayOperationSelected(operation);
}
