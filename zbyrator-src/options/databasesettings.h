#ifndef DATABASESETTINGS_H
#define DATABASESETTINGS_H

#include "gui-src/matildaconfwidget.h"


///[!] widgets-meters
#include "dataconcetrator-pgs/userdblimits.h"



///[!] sqliteclient
#include "matilda-bbb-src/database/sqlitelocalclient.h"



namespace Ui {
class DatabaseSettings;
}

class DatabaseSettings : public MatildaConfWidget
{
    Q_OBJECT

public:
    explicit DatabaseSettings(GuiHelper *gHelper, QWidget *parent = 0);
    ~DatabaseSettings();

signals:
    void startDroppingTables(QDateTime dtfrom, QDateTime dtto, quint8 code);

    void dropTables(QDateTime dtfrom, QDateTime dtto, quint8 code, qint64 len);


    //to db client
    void reStartTmrUpdater();

//to ipc
    void command2extension(quint16 extName, quint16 command, QVariant data);

public slots:
    void initPage();

    void reloadSettings();

    void saveSettings();

    void onDataDeletingEnd();


    void sendCurrentStatus(QString mess);
    void onTableCountingDone(qint64 count);

    void onTableDeletingDone(QString mess);
    void showMessDropper(QString mess);


    void setDbSizeBytes(qint64 bytes);

private slots:
    void on_pbDeleteDB_clicked();



    void on_tbResetDelSett_clicked();

private:
    Ui::DatabaseSettings *ui;

    void createTableDropper();

    void createDbLimitAndClient();

    UserDbLimits *dblimitswdgt;



    struct MyLastDropSett
    {
        QDateTime dtfrom;
        QDateTime dtto;
        quint8 pollcode;
        qint64 len;
        MyLastDropSett() {}
    } lDropSett;

};

class DbClientCover : public QObject
{
    Q_OBJECT
public:
    DbClientCover(QObject *parent = 0);
    virtual ~DbClientCover();

    SQliteLocalClient *dbClient;


    bool tmrIsRestarting;
signals:
    void setDbSizeBytes(qint64 bytes);

    void startTmrUpdater(int msec);

public slots:
    void onThreadStarted();

    void onTmrUpdater();

    void restartTmr();




};

#endif // DATABASESETTINGS_H
