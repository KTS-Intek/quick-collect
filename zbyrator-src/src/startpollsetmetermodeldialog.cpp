#include "startpollsetmetermodeldialog.h"
#include "ui_startpollsetmetermodeldialog.h"

#include <QTimer>

///[!] widgets-shared
#include "template-pgs/comboboxdelegate.h"

StartPollSetMeterModelDialog::StartPollSetMeterModelDialog(const QHash<QString, ClassManagerMeterInfo::MeterNi2info> &hashMeterNi2info, const QStringList &nis, const QStringList &models, QWidget *parent) :
    QDialog(parent), hashMeterNi2info(hashMeterNi2info), nis(nis), models(models),
    ui(new Ui::StartPollSetMeterModelDialog)
{
    ui->setupUi(this);
    ui->buttonBox->setEnabled(false);

    setWindowTitle(tr("Meter model correction"));

    QTimer::singleShot(111, this, SLOT(initThisPage()));
}

StartPollSetMeterModelDialog::~StartPollSetMeterModelDialog()
{
    delete ui;
}

QStringList StartPollSetMeterModelDialog::getOneRow(const QString &ni, const int &maxcols, QStringList &nosettnis)
{
    QStringList out;

    if(!hashMeterNi2info.contains(ni)){
        out.append("");
        out.append("");
        out.append(ni);
        nosettnis.append(ni);

    }else{

        const ClassManagerMeterInfo::MeterNi2info m = hashMeterNi2info.value(ni);
        out.append(m.minfo.model);
        out.append(m.sn);
        out.append(ni);
        out.append(m.minfo.memo);
    }

    for(int i = out.size(); i < maxcols; i++)
        out.append("");
    return out;

}

void StartPollSetMeterModelDialog::initThisPage()
{
    emit killCbxDelegate();

    QStringList nosettnis;

    ui->tableWidget->clear();

    const QStringList header = tr("Model;S/N;NI;Memo").split(";");

    const int columncount = header.size();
    ui->tableWidget->setColumnCount(columncount);
    for(int i = 0, imax = columncount; i < imax; i++)
        ui->tableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(header.at(i)));


    //    StandardItemModelHelper::setModelHorizontalHeaderItems(model, tr("Meter;SN;NI;Memo").split(";"));

    ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->tableWidget->setRowCount(nis.size());

    ComboBoxDelegate *c = new ComboBoxDelegate(Qt::UserRole, this) ;
    connect(this, SIGNAL(killCbxDelegate()), c, SLOT(deleteLater()) );
    ui->tableWidget->setItemDelegate(c);


    for(int i = 0, imax = nis.size(); i < imax; i++){


        const QStringList onerow = getOneRow(nis.at(i), columncount, nosettnis);

        for(int j = 0; j < columncount; j++){
            ui->tableWidget->setItem(i, j, new QTableWidgetItem(onerow.at(j)));
            ui->tableWidget->item(i,j)->setData(Qt::UserRole, onerow.at(j).split(" "));

        }


        ui->tableWidget->item(i,0)->setData(Qt::UserRole, models);



        //        StandardItemModelHelper::append2model(QStringList() << QString("%1, %2").arg(m.minfo.model).arg(m.minfo.vrsn) << m.sn << ni << m.minfo.memo, model);

    }

    ui->buttonBox->setEnabled(true);
    if(nosettnis.isEmpty())
         acceptSlot();//accept

}

void StartPollSetMeterModelDialog::on_buttonBox_accepted()
{
    sendSett();

}

void StartPollSetMeterModelDialog::acceptSlot()
{
    sendSett();
    accept();
}

void StartPollSetMeterModelDialog::sendSett()
{
    QStringList nisequence;
    QHash<QString,QString> ni2model;

    for(int i = 0, imax = ui->tableWidget->rowCount(); i < imax; i++){
        nisequence.append(ui->tableWidget->item(i, 2)->text());
        ni2model.insert(nisequence.last(), ui->tableWidget->item(i, 0)->text());

    }

    QStringList out;

    for(int i = 0, imax = nisequence.size(); i < imax; i++){
        const QString model = ni2model.value(nisequence.at(i));
        if(models.contains(model)){
            out.append(QString("%1&%2").arg(model).arg(nisequence.at(i)));
        }else{
            out.append(nisequence.at(i));
        }
    }
    emit setNi2model(out);

}
