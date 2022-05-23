#ifndef SELECTMETERS4POLL_H
#define SELECTMETERS4POLL_H

///[!] widgets-shared
#include "gui-src/wdgt/referencewidgetclassgui.h"


///[!] tasks-types
#include "src/zbyrator-v2/zbyratordatatypehelper.h"


///[!] quick-collect
#include "templates/selectmeters4polltopwdgt.h"


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


class SelectMeters4poll : public ReferenceWidgetClassGui
{
    Q_OBJECT

public:
    explicit SelectMeters4poll(GetDataFromDbDoneSignalizator *signalizator, GuiHelper *gHelper, QWidget *parent = nullptr);

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

    void allMeters2selectWdgtV2(MPrintTableOut table, QStringList humanHeader, QStringList listGrpsHuman);

//    void setPageSett(const MyListStringList &listRows, const QVariantMap &col2data, const QStringList &headerH, const QStringList &header, const bool &hasHeader);

    void onProcessingEnds(QStringList listMissingData);

private slots:
    void onpbPollAll_clicked();



    void onpbSelected_clicked();

    void onpbCheckedOn_clicked();

    void onCbxGroups_currentIndexChanged(const QString &arg1);

    void ontvTable_doubleClicked(const QModelIndex &index);

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

    SelectMeters4pollTopWdgt *topWdgt;
};



#endif // SELECTMETERS4POLL_H
