#include "databasesettings.h"
#include "ui_databasesettings.h"

#include <QTimer>
#include <QThread>
#include <QMessageBox>


#include "src/nongui/settloader.h"
#include "matilda-bbb-src/shared/pathsresolver.h"


///[!] quick-collect-gui-core
#include "quick-collect-gui-core/database/tabledropper.h"


#include "matildalimits.h"


DatabaseSettings::DatabaseSettings(GuiHelper *gHelper, QWidget *parent) :
    MatildaConfWidget(gHelper,  parent),
    ui(new Ui::DatabaseSettings)
{
    ui->setupUi(this);
    ui->pbDeleteDB->setEnabled(false);

}

DatabaseSettings::~DatabaseSettings()
{
    delete ui;
}

void DatabaseSettings::initPage()
{
    ui->lePathToDb->setText(PathsResolver::path2database());

    ui->sbLimitDBsize->setValue( SettLoader::loadSett(SETT_OPTIONS_DB_MAXSIZE, MAX_DATABASE_SIZE).toInt() / 1000000);//550 000 000 //~550M

    createTableDropper();

    ui->tbResetDelSett->animateClick();

    QTimer *t = new QTimer(this);
    t->setSingleShot(true);
    t->setInterval(555);
    connect(ui->sbLimitDBsize, SIGNAL(valueChanged(int)), t, SLOT(start()));
    connect(t, SIGNAL(timeout()), this, SLOT(saveSettings()));
}

void DatabaseSettings::saveSettings()
{
    if((SettLoader::loadSett(SETT_OPTIONS_DB_MAXSIZE, MAX_DATABASE_SIZE + 1).toInt() / 1000000) != ui->sbLimitDBsize->value())
        SettLoader::saveSett(SETT_OPTIONS_DB_MAXSIZE, ui->sbLimitDBsize->value() * 1000000);
}

void DatabaseSettings::onDataDeletingEnd()
{
    ui->pbDeleteDB->setEnabled(true);

}

void DatabaseSettings::sendCurrentStatus(QString mess)
{
    ui->lblDbMessages->setText(mess);

}

void DatabaseSettings::onTableCountingDone(qint64 count)
{
    lDropSett.len = count;
    if(count < 1){
        gHelper->showMessSlot(tr("There is nothing to delete"));
        return;
    }


    if(QMessageBox::question(this, windowTitle(), tr("Would you like to delete '%1' tables from the database?").arg(count)
                             , QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) == QMessageBox::No){
        return;

    }

    emit dropTables(lDropSett.dtfrom, lDropSett.dtto, lDropSett.pollcode, lDropSett.len);

}

void DatabaseSettings::onTableDeletingDone(QString mess)
{
    gHelper->showMessSlot(mess);
}

void DatabaseSettings::showMessDropper(QString mess)
{
    ui->lblDbMessages->setText(QString("<br>%1</b>").arg(mess));
}

void DatabaseSettings::on_pbDeleteDB_clicked()
{
    ui->pbDeleteDB->setEnabled(false);
    lDropSett.dtfrom = ui->dteDeleteFrom->dateTime();
    lDropSett.dtto = ui->dteDeleteTo->dateTime();
    lDropSett.pollcode = ui->sbPollCode->value();

    emit startDroppingTables(lDropSett.dtfrom, lDropSett.dtto, lDropSett.pollcode);

    //cbxDeleteFromDb

}

void DatabaseSettings::on_tbResetDelSett_clicked()
{
    const QDateTime dt = QDateTime::currentDateTime();
    ui->dteDeleteFrom->setDateTime(dt.addYears(-10));
    ui->dteDeleteTo->setDateTime(dt.addYears(-3));
    ui->sbPollCode->setValue(140);

}

void DatabaseSettings::createTableDropper()
{
    TableDropper *d = new TableDropper;
    QThread *t = new QThread;

    d->moveToThread(t);
    connect(t, SIGNAL(started()), d, SLOT(onThreadStarted()));
    connect(d, SIGNAL(destroyed(QObject*)), t, SLOT(quit()));
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
    connect(this, SIGNAL(destroyed(QObject*)), d, SLOT(deleteLater()));

    connect(d, SIGNAL(onDropperInitDone()), this, SLOT(onDataDeletingEnd()));
    connect(d, SIGNAL(operationDone()), this, SLOT(onDataDeletingEnd()));

    connect(d, &TableDropper::sendCurrentStatus, this, &DatabaseSettings::sendCurrentStatus);
    connect(d, &TableDropper::showMess, this, &DatabaseSettings::showMessDropper);
    connect(d, &TableDropper::onTableCountingDone, this, &DatabaseSettings::onTableCountingDone);
    connect(d, &TableDropper::onTableDeletingDone, this, &DatabaseSettings::onTableDeletingDone);


    connect(this, &DatabaseSettings::startDroppingTables, d, &TableDropper::startDroppingTables);
    connect(this, &DatabaseSettings::dropTables, d, &TableDropper::dropTables);

    t->start();
}
