#ifndef DATABASESETTINGS_H
#define DATABASESETTINGS_H

#include "gui-src/matildaconfwidget.h"




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


public slots:
    void initPage();

    void saveSettings();

    void onDataDeletingEnd();


    void sendCurrentStatus(QString mess);
    void onTableCountingDone(qint64 count);

    void onTableDeletingDone(QString mess);
    void showMessDropper(QString mess);


private slots:
    void on_pbDeleteDB_clicked();



    void on_tbResetDelSett_clicked();

private:
    Ui::DatabaseSettings *ui;

    void createTableDropper();

    struct MyLastDropSett
    {
        QDateTime dtfrom;
        QDateTime dtto;
        quint8 pollcode;
        qint64 len;
        MyLastDropSett() {}
    } lDropSett;

};

#endif // DATABASESETTINGS_H
