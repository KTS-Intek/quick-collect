#include "watersleepscheduleradder.h"
#include "ui_watersleepscheduleradder.h"
#include <QTimer>
#include "gui-src/settloader.h"
#include <QDebug>

WaterSleepSchedulerAdder::WaterSleepSchedulerAdder(QWidget *parent) :
    ConfPopupWdgt(SettLoader::getPopupSett(), parent),
    ui(new Ui::WaterSleepSchedulerAdder)
{
    ui->setupUi(this);


    addMoveHoldTb(ui->verticalLayout_2, ui->cbxNI->height());

    if(true){
        QTimer *t = new QTimer(this);
        t->setSingleShot(true);
        t->setInterval(111);

        connect(ui->cbxNI, SIGNAL(currentIndexChanged(int)), t, SLOT(start()));
        //    connect(ui->cbxNI, SIGNAL(currentTextChanged(QString)), t, SLOT(start()));

        connect(ui->cbxNI, SIGNAL(editTextChanged(QString)), t, SLOT(start()));

        //    connect(ui->cbxProfile, SIGNAL(currentIndexChanged(int)), t, SLOT(start()));

        connect(t, SIGNAL(timeout()), this, SLOT(updateModelLbl()));
    }




}

WaterSleepSchedulerAdder::~WaterSleepSchedulerAdder()
{
    delete ui;
}



void WaterSleepSchedulerAdder::onTbClearPressed()
{
    pgsett.cbxNi = ui->cbxNI->currentText();
    pgsett.cbxProfile = ui->cbxProfile->currentText();
    pgsett.cbxProfLineSmpl = ui->cbxProfile->currentData().toString();
    pgsett.hasSomeData = "1";

    clearPage();
    emit onHasUndoData(true);
}

void WaterSleepSchedulerAdder::onTbUndoPressed()
{
    emit onHasUndoData(false);
    if(pgsett.hasSomeData.isEmpty())
        return;
    setPageSett(pgsett.cbxNi, pgsett.cbxProfile, pgsett.cbxProfLineSmpl);

}

void WaterSleepSchedulerAdder::clearPage()
{
    ui->cbxNI->setCurrentIndex(-1);
    ui->cbxProfile->setCurrentIndex(ui->cbxProfile->count() - 1);

}

void WaterSleepSchedulerAdder::addNi()
{

    ui->cbxNI->setCurrentIndex(-1);
    emit sendMeNewMaps();

}

void WaterSleepSchedulerAdder::setPageSett(const QString &ni, const QString &profName, const QString &profLineSmpl)
{
    emit sendMeNewMaps();
    ui->cbxNI->setCurrentIndex(ui->cbxNI->findText(ni));

    if(!ni.isEmpty() && ui->cbxNI->currentIndex() < 0 )
        ui->cbxNI->setCurrentText(ni);

    const int indx = ui->cbxProfile->findData(profLineSmpl);
    ui->cbxProfile->setCurrentIndex( (indx < 0) ? ui->cbxProfile->findText(profName) : indx);
    pgsett.hasSomeData.clear();

}

void WaterSleepSchedulerAdder::setMaps(const QMap<QString, QString> &ni2model, const QMap<QString, QString> &ni2lastProfile)
{

    this->ni2model = ni2model;
    if(true){
        const QString ni = ui->cbxNI->currentText();
        ui->cbxNI->clear();

        QList<QString> lk = ni2model.keys();
        std::sort(lk.begin(), lk.end());
        for(int i = 0, imax = lk.size(); i < imax; i++)
            ui->cbxNI->addItem(lk.at(i), ni2model.value(lk.at(i)));

        ui->cbxNI->setCurrentIndex(ui->cbxNI->findText(ni));
    }

    if(true){
        const QString prof = ui->cbxProfile->currentText();
        ui->cbxProfile->clear();

        QList<QString> lk = ni2lastProfile.keys();
        std::sort(lk.begin(), lk.end());
        for(int i = 0, imax = lk.size(); i < imax; i++)
            ui->cbxProfile->addItem(lk.at(i), ni2lastProfile.value(lk.at(i)));

        ui->cbxProfile->setCurrentIndex(ui->cbxProfile->findText(prof));
    }
}

void WaterSleepSchedulerAdder::updateModelLbl()
{
    const QString model = ni2model.value(ui->cbxNI->currentText()).isEmpty() ? tr("Auto") : ni2model.value(ui->cbxNI->currentText());
//    const QString profile = ui->cbxProfile->currentData().tos

    ui->lblModel->setText(model);
}

bool WaterSleepSchedulerAdder::addNiProfile()
{
    const QString ni = QString(ui->cbxNI->currentText()).simplified().remove(" ");
    if(ni.isEmpty())
        return false;

    if(ui->cbxProfile->currentText().isEmpty())
        return false;

    emit onAddNi(ni, ui->cbxProfile->currentText(), ui->cbxProfile->currentData().toString());
    return true;

}

void WaterSleepSchedulerAdder::on_pbAdd_clicked()
{
   addNiProfile();
}

void WaterSleepSchedulerAdder::on_pbAddAndHide_clicked()
{
    if(addNiProfile())
        hide();
}
