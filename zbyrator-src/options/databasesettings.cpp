#include "databasesettings.h"
#include "ui_databasesettings.h"

#include <QTimer>
#include <QThread>
#include <QMessageBox>


#include "src/nongui/settloader.h"
#include "matilda-bbb-src/shared/pathsresolver.h"


///[!] quick-collect-gui-core
#include "quick-collect-gui-core/database/tabledropper.h"



///[!] matilda-bbb-settings
#include "src/matilda/settloader4matilda.h"


#include "matildalimits.h"
#include "moji_defy.h"

//-----------------------------------------------------------------

DatabaseSettings::DatabaseSettings(GuiHelper *gHelper, QWidget *parent) :
    MatildaConfWidget(gHelper,  parent),
    ui(new Ui::DatabaseSettings)
{
    ui->setupUi(this);
    ui->pbDeleteDB->setEnabled(false);

    createDbLimitAndClient();
}

//-----------------------------------------------------------------

DatabaseSettings::~DatabaseSettings()
{
    saveSettings();
    delete ui;
}

//-----------------------------------------------------------------

void DatabaseSettings::initPage()
{
    ui->lePathToDb->setText(PathsResolver::path2database());

//    ui->sbLimitDBsize->setValue( SettLoader::loadSett(SETT_OPTIONS_DB_MAXSIZE, MAX_DATABASE_SIZE).toInt() / 1000000);//550 000 000 //~550M

    createTableDropper();

    ui->tbResetDelSett->animateClick();

    QTimer *t = new QTimer(this);
    t->setSingleShot(true);
    t->setInterval(55555);

    connect(dblimitswdgt, SIGNAL(onSettingsChanged()), t, SLOT(start()));
    connect(t, SIGNAL(timeout()), this, SLOT(saveSettings()));
}

//-----------------------------------------------------------------

void DatabaseSettings::reloadSettings()
{
    QVariantHash h = SettLoader4matilda().loadOneSett(SETT_DB_LIMITS).toHash();
    if(h.isEmpty()){
        //    ui->sbLimitDBsize->setValue( SettLoader::loadSett(SETT_OPTIONS_DB_MAXSIZE, MAX_DATABASE_SIZE).toInt() / 1000000);//550 000 000 //~550M

        //try to import old settings
        const int mb = SettLoader::loadSett(SETT_OPTIONS_DB_MAXSIZE, MAX_DATABASE_SIZE).toInt();
        if(mb >= MAX_DATABASE_RESERV_SIZE_FREE_DISK){
            SettLoader::saveSett(SETT_OPTIONS_DB_MAXSIZE, -1);

            h.insert("mb", mb);
             SettLoader4matilda().saveOneSett(SETT_DB_LIMITS, h);
        }
    }


    h.insert("maxmb", MAX_DATABASE_SIZE/1000000U); //compiled max db size
    h.insert("minmb", (MAX_DATABASE_RESERV_SIZE_FREE_DISK + MAX_DATABASE_RESERV_SIZE)/1000000U); //compiled min db size


     QFileInfo fi(QString(PathsResolver::path2database()));
     h.insert("dbf", fi.size());
     h.insert("dbd", -1);


//     dblimitswdgt->setPageSett(h);


}

//-----------------------------------------------------------------

void DatabaseSettings::saveSettings()
{
    dblimitswdgt->hideLblMessage();

    const QVariantHash savedh = SettLoader4matilda().loadOneSett(SETT_DB_LIMITS).toHash();

    bool ok;
    QString message;
    const QVariantHash h ;// = dblimitswdgt->getPageSett(ok, message, false);
    if(ok && savedh != h){
        SettLoader4matilda().saveOneSett(SETT_DB_LIMITS, h);

        //reload settings to sqlite-medium
        emit command2extension(MTD_EXT_NAME_SQLITE_MEDIUM_SERVER, MTD_EXT_COMMAND_RELOAD_SETT, true);
    }else{
        qDebug() << "DatabaseSettings::saveSettings() " << ok << message << h << savedh;
    }



//    if((SettLoader::loadSett(SETT_OPTIONS_DB_MAXSIZE, MAX_DATABASE_SIZE + 1).toInt() / 1000000) != ui->sbLimitDBsize->value())
//        SettLoader::saveSett(SETT_OPTIONS_DB_MAXSIZE, ui->sbLimitDBsize->value() * 1000000);
}

//-----------------------------------------------------------------

void DatabaseSettings::onDataDeletingEnd()
{
    ui->pbDeleteDB->setEnabled(true);

}

//-----------------------------------------------------------------

void DatabaseSettings::sendCurrentStatus(QString mess)
{
    ui->lblDbMessages->setText(mess);

}

//-----------------------------------------------------------------

void DatabaseSettings::onTableCountingDone(qint64 count)
{
    lDropSett.len = count;
    if(count < 1){
        gHelper->showMessageSlot(tr("There is nothing to delete"));
        return;
    }


    if(QMessageBox::question(this, windowTitle(), tr("Would you like to delete '%1' tables from the database?").arg(count)
                             , QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) == QMessageBox::No){
        return;

    }

    emit dropTables(lDropSett.dtfrom, lDropSett.dtto, lDropSett.pollcode, lDropSett.len);

}

//-----------------------------------------------------------------

void DatabaseSettings::onTableDeletingDone(QString mess)
{
    gHelper->showMessageSlot(mess);
}

//-----------------------------------------------------------------

void DatabaseSettings::showMessDropper(QString mess)
{
    ui->lblDbMessages->setText(QString("<br>%1</b>").arg(mess));
}

//-----------------------------------------------------------------

void DatabaseSettings::setDbSizeBytes(qint64 bytes)
{
    QVariantHash h ;

     QFileInfo fi(QString(PathsResolver::path2database()));
     h.insert("dbf", fi.size());
     h.insert("dbd", bytes);
//     dblimitswdgt->setPageDbSize(h);
}

//-----------------------------------------------------------------

void DatabaseSettings::on_pbDeleteDB_clicked()
{
    ui->pbDeleteDB->setEnabled(false);
    lDropSett.dtfrom = ui->dteDeleteFrom->dateTime();
    lDropSett.dtto = ui->dteDeleteTo->dateTime();
    lDropSett.pollcode = ui->sbPollCode->value();

    emit startDroppingTables(lDropSett.dtfrom, lDropSett.dtto, lDropSett.pollcode);

    //cbxDeleteFromDb

}

//-----------------------------------------------------------------

void DatabaseSettings::on_tbResetDelSett_clicked()
{
    const QDateTime dt = QDateTime::currentDateTime();
    ui->dteDeleteFrom->setDateTime(dt.addYears(-10));
    ui->dteDeleteTo->setDateTime(dt.addYears(-3));
    ui->sbPollCode->setValue(140);

}

//-----------------------------------------------------------------

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

//-----------------------------------------------------------------

void DatabaseSettings::createDbLimitAndClient()
{

    DbClientCover *cover = new DbClientCover;

    QThread *t = new QThread;

    cover->moveToThread(t);

    connect(this, SIGNAL(destroyed(QObject*)), cover, SLOT(deleteLater()));
    connect(cover, SIGNAL(destroyed(QObject*)), t, SLOT(quit()));
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));

    connect(t, &QThread::started, cover, &DbClientCover::onThreadStarted);
    connect(this, &DatabaseSettings::reStartTmrUpdater, cover, &DbClientCover::restartTmr);
    connect(cover, &DbClientCover::setDbSizeBytes, this, &DatabaseSettings::setDbSizeBytes);



    dblimitswdgt = new UserDbLimits(gHelper, this);
    ui->vl4dbLimitsWdgt->addWidget(dblimitswdgt);


    connect(dblimitswdgt, &UserDbLimits::pageEndInit, this, &DatabaseSettings::reloadSettings);
    connect(dblimitswdgt, SIGNAL(pageEndInit()), t, SLOT(start()));


}

DbClientCover::DbClientCover(QObject *parent) : QObject(parent)
{
    tmrIsRestarting = true;
}

DbClientCover::~DbClientCover()
{


}

void DbClientCover::onThreadStarted()
{
    QTimer *tmr = new QTimer(this);
    tmr->setInterval(1111);
    tmr->setSingleShot(true);

    connect(this, SIGNAL(startTmrUpdater(int)), tmr, SLOT(start(int)));
    connect(tmr, &QTimer::timeout, this, &DbClientCover::onTmrUpdater);



    dbClient = new SQliteLocalClient( PathsResolver::defSqliteMediumLocalServerName() , true, this);
//        connect(this, SIGNAL(killSqliteMedium()), dbClient, SLOT(stopAllSlot())) ;

//    connect(dbClient, SIGNAL(mWriteIneedMoreTime(quint16,qint32,qint64)), this, SIGNAL(mWriteIneedMoreTime(quint16,qint32,qint64)) );
//    connect(this, SIGNAL(setIneedMoreTime(quint16,qint32,qint64)), dbClient, SLOT(setIneedMoreTime(quint16,qint32,qint64)) );

//    connect(this, SIGNAL(stopAllNow()), dbClient, SLOT(stopAllSlotDirect()) , Qt::DirectConnection);
//    connect(this, SIGNAL(stopAllNow()), dbClient, SLOT(stopAllSlot()) );

    dbClient->initObj();
    dbClient->setRegisterAutomatically(true);
    dbClient->setDefaultTimeoutG(3333);

    tmrIsRestarting = false;

    restartTmr();
}

void DbClientCover::onTmrUpdater()
{
    tmrIsRestarting = false;
    const qint64 bytes = dbClient->getDataBaseDataSize(false, 7777);

    emit setDbSizeBytes(bytes);

    restartTmr();
}

void DbClientCover::restartTmr()
{
    if(tmrIsRestarting)
        return;
    tmrIsRestarting = true;
    emit startTmrUpdater(3333);
}
