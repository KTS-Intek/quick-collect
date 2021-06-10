#ifndef SELECTMETERS4POLL_H
#define SELECTMETERS4POLL_H

///[!] widgets-shared
#include "gui-src/referencewidgetclass.h"

///[!] tasks-types
#include "src/zbyrator-v2/zbyratordatatypehelper.h"

class   GetDataFromDbDoneSignalizator : public QObject
{
    Q_OBJECT
public:
    explicit  GetDataFromDbDoneSignalizator(QObject *parent = nullptr);
    virtual ~  GetDataFromDbDoneSignalizator() {}

signals:
    void onDbSelectorKickedOff();
    void stopDbSelector();


};

namespace Ui {
class SelectMeters4poll;
}

class SelectMeters4poll : public ReferenceWidgetClass
{
    Q_OBJECT

public:
    explicit SelectMeters4poll(GetDataFromDbDoneSignalizator *signalizator, GuiHelper *gHelper, QWidget *parent = nullptr);
    ~SelectMeters4poll();

    GetDataFromDbDoneSignalizator *signalizator;
signals:
    void onReloadAllMeters();

    void onAllMeters(UniversalMeterSettList allMeters);

//    void command4dev(quint16 command, QString args);//pollCode args
    void command4dev(quint16 command, QVariantMap mapArgs);//pollCode args

public slots:
    void setPollSett(const QDateTime &dtFrom, const QDateTime &dtTo, const quint8 &pollCode, const quint8 &deviceType, const int &go2sleepSeconds, const bool &enCheckSleepProfile, const bool &ignoreRetr);

    void setPollSettElectric(const QDateTime &dtFrom, const QDateTime &dtTo, const quint8 &pollCode, const bool &ignoreRetr);

    void setPollSettWater(const QDateTime &dtFrom, const QDateTime &dtTo, const quint8 &pollCode, const bool &enSleepCommand, const int &go2sleepSeconds, const bool &enCheckSleepProfile, const bool &ignoreRetr);


    void setPollSettGas(const QDateTime &dtFrom, const QDateTime &dtTo, const quint8 &pollCode, const bool &ignoreRetr);

    void setPollSettPulse(const QDateTime &dtFrom, const QDateTime &dtTo, const quint8 &pollCode, const bool &ignoreRetr);

    void initPage();


//    void setPageSett(const MyListStringList &listRows, const QVariantMap &col2data, const QStringList &headerH, const QStringList &header, const bool &hasHeader);

    void onProcessingEnds(QStringList listMissingData);

private slots:
    void on_pbPollAll_clicked();



    void on_pbSelected_clicked();

    void on_pbCheckedOn_clicked();

    void onCbxGroups_currentIndexChanged(const QString &arg1);

    void on_tvTable_doubleClicked(const QModelIndex &index);

private:
    void sendStartPoll(const QStringList &listni);

    struct LastPollSett{
        QDateTime dtFrom;
        QDateTime dtTo;
        quint8 pollCode;
        quint8 deviceType;
//        bool enablePowerManagement;
        int go2sleepSeconds;
        bool enCheckSleepProfile;
        bool ignoreRetr;
    } lPollSett;

    Ui::SelectMeters4poll *ui;
};



#endif // SELECTMETERS4POLL_H
