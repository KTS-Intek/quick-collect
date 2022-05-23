#include "selectmeters4polltopwdgt.h"
#include "ui_selectmeters4polltopwdgt.h"

SelectMeters4pollTopWdgt::SelectMeters4pollTopWdgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectMeters4pollTopWdgt)
{
    ui->setupUi(this);
}

SelectMeters4pollTopWdgt::~SelectMeters4pollTopWdgt()
{
    delete ui;
}

QPushButton *SelectMeters4pollTopWdgt::getPbCancel()
{
    return ui->pbCancel;
}

QPushButton *SelectMeters4pollTopWdgt::getPbPollAll()
{
    return ui->pbPollAll;
}

QPushButton *SelectMeters4pollTopWdgt::getPbSelected()
{
    return ui->pbSelected;
}

QPushButton *SelectMeters4pollTopWdgt::getPbCheckedOn()
{
    return ui->pbCheckedOn;
}

QCheckBox *SelectMeters4pollTopWdgt::getCbxIgnoreExistingData()
{
    return ui->cbxIgnoreExistingData;
}

QLabel *SelectMeters4pollTopWdgt::getLblDatabase()
{
    return ui->lblDatabase;
}

QComboBox *SelectMeters4pollTopWdgt::getCbxGroups()
{
    return ui->cbxGroups;
}
