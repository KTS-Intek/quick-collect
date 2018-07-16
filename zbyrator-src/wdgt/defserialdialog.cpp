#include "defserialdialog.h"
#include "ui_defserialdialog.h"

DefSerialDialog::DefSerialDialog(QStringList list, QStringList listT, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DefSerialDialog)
{
    ui->setupUi(this);
    for(int i = 0, iMax = list.size(); i < iMax; i++){
        ui->comboBox->addItem(list.at(i));
        ui->comboBox->setItemData(i, listT.at(i), Qt::ToolTipRole);
    }

}

DefSerialDialog::~DefSerialDialog()
{
    delete ui;
}

void DefSerialDialog::on_buttonBox_accepted()
{
    emit setThisPortAsDefault(ui->comboBox->currentText());
}

