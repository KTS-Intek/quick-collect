#include "waterprofilewdgt.h"
#include "ui_waterprofilewdgt.h"
#include "template-pgs/comboboxdelegate.h"
#include <QTimer>

WaterProfileWdgt::WaterProfileWdgt(const bool &enableEditor, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WaterProfileWdgt)
{
    ui->setupUi(this);
    lProfile.enableEditor = enableEditor;


    QTimer *t = new QTimer(this);
    t->setSingleShot(true);
    t->setInterval(333);

    connect(ui->tableWidget, SIGNAL(cellChanged(int,int)), t, SLOT(start()) );
    connect(t, SIGNAL(timeout()), this, SLOT(updatePteText()) );
}

WaterProfileWdgt::~WaterProfileWdgt()
{
    delete ui;
}

QStringList WaterProfileWdgt::getDowNames()
{
    return tr("Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday").split(";");
}

QVariantHash WaterProfileWdgt::getDefault()
{
    QVariantHash h;
    h.insert("dow", 1);//monday
    h.insert("dom", 0xFF);//any
    h.insert("hour", 10);
    h.insert("minute", 0);
    h.insert("actvt", 10);
    return h;
}

QVariantHash WaterProfileWdgt::getProfile()
{
    const QStringList numberList = ComboBoxDelegate::generateListFromTo(0, 59, "");
    QVariantHash h;
    h.insert("dow"   , lProfile.dowNames.contains(ui->tableWidget->item(0,0)->text()) ? lProfile.dowNames.indexOf(ui->tableWidget->item(0,0)->text()) : 0xFF);
    h.insert("dom"   , numberList.contains(ui->tableWidget->item(0, 1)->text()) ? ui->tableWidget->item(0, 1)->text() : "255");
    h.insert("hour"  , numberList.contains(ui->tableWidget->item(0, 2)->text()) ? ui->tableWidget->item(0, 2)->text() : "255");
    h.insert("minute", numberList.contains(ui->tableWidget->item(0, 3)->text()) ? ui->tableWidget->item(0, 3)->text() : 0);
    h.insert("actvt" , ComboBoxDelegate::generateListFromTo(1, 255, "").contains(ui->tableWidget->item(0, 4)->text()) ? ui->tableWidget->item(0, 4)->text() : "3");
    return h;
}

void WaterProfileWdgt::setProfile(const QVariantHash &profile)
{
    disconnect(ui->tableWidget, SIGNAL(cellChanged(int,int)), this, SIGNAL(onTableChanged()) );

    ui->tableWidget->clear();
    emit killCbxDelegate();

    const QStringList header = tr("Day of the week;Day of the month;Hour;Minute;Time of the activity").split(";");
    ui->tableWidget->setColumnCount(header.size());
    ui->tableWidget->setRowCount(1);
    if(lProfile.enableEditor){
        ComboBoxDelegate *c = new ComboBoxDelegate(Qt::UserRole, this) ;
        connect(this, SIGNAL(killCbxDelegate()), c, SLOT(deleteLater()) );
        ui->tableWidget->setItemDelegate(c);


        ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
    }else{
        ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    for(int i = 0, imax = header.size(); i < imax; i++)
        ui->tableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(header.at(i)));


    lProfile.dowNames = getDowNames();
    lProfile.anyLocalTxt = tr("Any");



    const QVariantHash defProfile = getDefault();

    QStringList dowNamesL = lProfile.dowNames;
    dowNamesL.prepend(lProfile.anyLocalTxt);

    QTableWidgetItem *item0 = new QTableWidgetItem( (profile.value("dow", 0xFF).toUInt() > 6) ? lProfile.anyLocalTxt : lProfile.dowNames.at(profile.value("dow").toInt()));
    item0->setData(Qt::UserRole, dowNamesL);
    ui->tableWidget->setItem(0, 0, item0);


    QTableWidgetItem *item1 = new QTableWidgetItem( (profile.value("dom", 0xFF).toUInt() > 31) ? lProfile.anyLocalTxt : profile.value("dom").toString());
    item1->setData(Qt::UserRole, ComboBoxDelegate::generateListFromTo(1, 31, lProfile.anyLocalTxt));
    ui->tableWidget->setItem(0, 1, item1);

    QTableWidgetItem *item2 = new QTableWidgetItem( (profile.value("hour", 0xFF).toUInt() > 23) ? lProfile.anyLocalTxt : profile.value("hour").toString());
    item2->setData(Qt::UserRole, ComboBoxDelegate::generateListFromTo(0, 23, lProfile.anyLocalTxt));
    ui->tableWidget->setItem(0, 2, item2);

    QTableWidgetItem *item3 = new QTableWidgetItem( (profile.value("minute", 0xFF).toUInt() > 59) ? defProfile.value("minute").toString() : profile.value("minute").toString());
    item3->setData(Qt::UserRole, ComboBoxDelegate::generateListFromTo(0, 59, ""));
    ui->tableWidget->setItem(0, 3, item3);

    QTableWidgetItem *item4 = new QTableWidgetItem( (profile.value("actvt", 0).toUInt() == 0) ? defProfile.value("actvt").toString() : profile.value("actvt").toString());
    item4->setData(Qt::UserRole, ComboBoxDelegate::generateListFromTo(1, 254, ""));
    ui->tableWidget->setItem(0, 4, item4);

    connect(ui->tableWidget, SIGNAL(cellChanged(int,int)), this, SIGNAL(onTableChanged()) );

    QTimer::singleShot(111, this, SLOT(updatePteWidth()) );
    emit onNewProfile(getProfile());
}

void WaterProfileWdgt::setDefaultValues()
{
    setProfile(getDefault());
}

void WaterProfileWdgt::saveProfile(QString name)
{
    emit onSaveProfile(name, getProfile());
}

void WaterProfileWdgt::updatePteText()
{
    const QVariantHash h = getProfile();
    ui->plainTextEdit->clear();
    const QStringList lk = QString("dow dom hour minute actvt").split(" ", QString::SkipEmptyParts);
    QStringList l;
    for(int i = 0, imax = lk.size(); i < imax; i++)
        l.append(QString("%1: %2").arg(lk.at(i)).arg(h.value(lk.at(i)).toString()));
    ui->plainTextEdit->appendPlainText(l.join("\n"));
}

void WaterProfileWdgt::updatePteWidth()
{
    ui->plainTextEdit->setMaximumWidth(ui->plainTextEdit->height());

}


