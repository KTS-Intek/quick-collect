#include "editwaterprofile.h"
#include "ui_editwaterprofile.h"
#include "gui-src/settloader.h"
#include "waterprofilewdgt.h"
#include "zbyrator-water/src/watersleepschedulesaver.h"
#include <QDateTime>
#include <QTimeZone>


EditWaterProfile::EditWaterProfile(QWidget *parent) :
    ConfPopupWdgt(SettLoader::getPopupSett(), parent),
    ui(new Ui::EditWaterProfile)
{
    ui->setupUi(this);



    QDateTime dt = QDateTime::currentDateTime();
    const int offset = dt.offsetFromUtc();
    int offset2 = offset;
    for(int i = 1; i < 12 && offset == offset2; i++)
        offset2 = dt.addMonths(i).offsetFromUtc();

    const bool hasDst = dt.timeZone().hasDaylightTime();
    dt = dt.toUTC();
    dt.setOffsetFromUtc(offset);

    QString user;
    if(hasDst){
        const QString now = dt.timeZoneAbbreviation();
        dt.setOffsetFromUtc(offset2);
        user = tr("You are in the %1 and %2").arg(now).arg(dt.timeZoneAbbreviation());
    }else{
        user = tr("You are in the %1").arg(dt.timeZoneAbbreviation());
    }

    ui->lblTz->setText(tr("<b>Meters and schedule are in the UTC±00:00 timezone.<br>%1</b>").arg(user));

}

EditWaterProfile::~EditWaterProfile()
{
    delete ui;
}


void EditWaterProfile::on_pbDefault_clicked()
{
//    ui->pbDefault->setEnabled(false);
}

void EditWaterProfile::setProfileSlot(const QString &name, const QVariantHash &profile)
{
//    ui->pbDefault->setEnabled(false);

    ui->lineEdit->setText(name);
    if(!name.isEmpty())
        emit setProfile(profile);
    else if(name.isEmpty() && lastProfile.isEmpty())
        emit setProfile(WaterSleepScheduleSaver::getDefaultProfile());


    lastProfileName = name;
    lastProfile = profile;

    showLater();

//    ui->pbSave->setEnabled(false);
}

void EditWaterProfile::onNewProfile(const QVariantHash &profile)
{
    ui->pbDefault->setEnabled(profile != WaterSleepScheduleSaver::getDefaultProfile());
}

void EditWaterProfile::onTbClearPressed()
{
    ui->lineEdit->clear();
    emit setProfile(QVariantHash());
    emit onHasUndoData(true);

}

void EditWaterProfile::onTbUndoPressed()
{
    emit onHasUndoData(false);
    ui->lineEdit->setText(lastProfileName);
    emit setProfile(lastProfile);
}

void EditWaterProfile::setupPage()
{
    WaterProfileWdgt *profileWdgt = new WaterProfileWdgt(true, this);
    ui->vl4wdgt->addWidget(profileWdgt);

    connect(this, SIGNAL(setProfile(QVariantHash)), profileWdgt, SLOT(setProfile(QVariantHash)));
    connect(ui->pbDefault, SIGNAL(clicked(bool)), profileWdgt, SLOT(setDefaultValues()));
    connect(this, SIGNAL(setProfileName(QString)), profileWdgt, SLOT(saveProfile(QString)) );

    connect(profileWdgt, SIGNAL(onSaveProfile(QString,QVariantHash)), this, SIGNAL(onSaveProfile(QString,QVariantHash)) );

    connect(profileWdgt, SIGNAL(onSaveProfile(QString,QVariantHash)), this, SLOT(hide()) );
    connect(profileWdgt, SIGNAL(onNewProfile(QVariantHash)), this, SLOT(onNewProfile(QVariantHash)) );

    connect(this, SIGNAL(deleteProfileName()), this, SLOT(hide()) );

    ui->pbDefault->setEnabled(true);
    ui->pbSave->setEnabled(true);

    addMoveHoldTb(ui->verticalLayout, ui->pbSave->height());
    emit ready2edit();
}


void EditWaterProfile::on_pbSave_clicked()
{
    if(!ui->lineEdit->text().isEmpty())
        emit setProfileName(ui->lineEdit->text());

}



