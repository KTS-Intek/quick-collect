#ifndef ZBYRMETERLISTMEDIUM_H
#define ZBYRMETERLISTMEDIUM_H

///[!] widgets-shared
#include "gui-src/guiifacemedium.h"


///[!] task-types
#include "src/zbyrator-v2/zbyratordatatypehelper.h"


///[!] guisett-shared
#include "src/nongui/classmanagermeterinfo.h"


///[!] quick-collect-gui-core
#include "quick-collect-gui-core/meters/lastmetersstatusesmanager.h"



#include "classmanagertypes.h"

#include <QLineEdit>

//#include "zbyrator-src/src/ifacesettloader.h"
//#include "src/matilda-conf/classmanagersharedobjects.h"

class ZbyrMeterListMedium : public GuiIfaceMedium
{
    Q_OBJECT
public:
    explicit ZbyrMeterListMedium(QObject *parent = nullptr);

    void importGroups2metersFile();

    LastMetersStatusesManager *metersStatusManager;

    void resetVariables4pollStarted();
signals:
    void setElectricityMeterListPageSett(MyListStringList listRows, QVariantMap col2data, QStringList headerH, QStringList header, bool hasHeader);

    void setWaterMeterListPageSett(MyListStringList listRows, QVariantMap col2data, QStringList headerH, QStringList header, bool hasHeader);

    void setWaterMeterSchedulerPageSett(MyListStringList listRows, QVariantMap col2data, QStringList headerH, QStringList header, bool hasHeader);


    void setRelayPageSett(MyListStringList listRows, QVariantMap col2data, QStringList headerH, QStringList header, bool hasHeader);

    void setDateTimePageSett(MyListStringList listRows, QVariantMap col2data, QStringList headerH, QStringList header, bool hasHeader);

    void setStatisticOfExchangePageSett(QVariantHash h);

    void realoadMeters2meterManager();//if changed NI, model, SN, removed meter, added meter,


    void startTmrSaveLater();

    void onConfigChanged(quint16 command, QVariant data);

    //meterlistwdgt
    void onReloadAllMeters2zbyrator();


    void sendMeAlistOfMeters(quint8 meterType);



    void onUpdatedSavedList(int activeMetersSize, int switchedOffMetersSize, int meterElectricityActive, int metersWaterActive);




    //to mainwindow
    void openEditMacProfileWdgt(bool isEditMode, QLineEdit *le );



    void onAddMeters(quint8 meterType, UniversalMeterSettList activeMeters, MyNi2model switchedOffMeters, bool checkOffMeters);



//    void updateHashSn2meter(QHash<QString,QString> hashMeterSn2memo, QHash<QString,QString> hashMeterSn2ni, QHash<QString, QString> hashMeterNi2memo);
//    void updateHashSn2meter(QHash<QString,QString> hashMeterSn2memo, QHash<QString,QString> hashMeterSn2ni, QHash<QString, ClassManagerMeterInfo::MeterNi2info> hashMeterNi2info, QStringList listnis);
    void updateHashSn2meter(QHash<QString,QString> hashMeterSn2memo, QHash<QString,QString> hashMeterSn2ni, QHash<QString, ClassManagerMeterInfo::MeterNi2info> hashMeterNi2info, QStringList listnis, quint8 meterType);


    void onAllMeters(UniversalMeterSettList allMeters);

    void onStatisticChanged(QString pollCodeStr, QStringList oneRow);

    void onCOMMAND_READ_POLL_STATISTIC(QStringList list);


    void onZbyratorConfigChanged(quint16 command, QVariant dataVar);




//    void meterRelayStatus(QString ni, QDateTime dtLocal, QString stts);
    void meterRelayStatus(QString ni, QDateTime dtLocal, quint8 mainstts, quint8 secondarystts);
    void add2fileMeterRelayStatus(QString ni, QDateTime dtLocal, quint8 mainstts, quint8 secondarystts);//2 cache


    void meterDateTimeDstStatus(QString ni, QDateTime dtLocal, QString stts);

    void waterMeterSchedulerStts(QString ni, QDateTime dtLocal, QString stts, QVariantHash sheduler, QString src);

    void reloadSavedSleepProfiles();




//Database and meterjournal
    void data2dbMedium(quint16 command, QVariant varData);
    void stopReadDatabase();

    void appendDataDatabase(QVariantHash hash);

    void appendDataDatabaseMJ(QVariantHash hash);
    void setPbReadEnableDisable(bool disable);
    void setLblWaitTxtDatabase(QString s);
    void setLblWaitTxtDatabaseMj(QString s);


    void setDateMask(QString dateMask);
    void setDotPos(int dotPos);


    void reloadSettings();


    //zbyrator ipc
    void command2extensionClient(quint16 command, QVariant dataVar);

    void onAboutZigBee(QVariantHash hash);

    void relayStatusChanged(QVariantMap map);

    void setWaterPowerCenters(const QVariantList &meters);
    void setElectricityPowerCenters(const QVariantList &meters);


    void pbStopAnimateClick();
    void onExternalCommandProcessed();

public slots:
    void onAllMetersSlot(UniversalMeterSettList allMeters);

    void meterElectricityModelChanged(QVariantList meters);
    void meterWaterModelChanged(QVariantList meters);

    void onSaveLater();

    void doReloadListOfMeters(quint8 meterType);

    void onAlistOfMeters(quint8 meterType, UniversalMeterSettList activeMeters, MyNi2model switchedOffMeters);



    void createDataCalculator();

    void createDatabaseMedium();


    void onAllStatHash(QStringList allstat);

    void onPollCodeChangedStat(QVariantHash hash);

    void onTaskCanceled(quint8 pollCode, QString ni, qint64 dtFinished, quint8 rez);

    void setLastPageId(QString accsblName);

    void onReloadAllMeters();

    void onReloadAllZbyratorSettingsLocalSocket();

    void createPeredavatorEmbeeManagerLater();

    void command4devSlotLocalSocket(quint16 command, QString args);//pollCode args

    void sendCachedDataAboutRelays(const QStringList &niswithoutsttses);

private:
    struct LastList2pages
    {
        QStringList listNI;
        QStringList mainParams; //model version SN
        LastList2pages() {}
    } ;

    QMap<QString, LastList2pages> mapMeters2pages;

    QStringList universalMeterSett2listRow(const UniversalMeterSett &m, QStringList &ldata, QList<int> &lcols);

    QStringList universalMeterSett2listRowElectricity(const UniversalMeterSett &m);

    QStringList universalMeterSett2listRowWater(const UniversalMeterSett &m, QStringList &ldata, QList<int> &lcols);


    bool metersChanged(QMap<QString, LastList2pages> &mapMeters2pages, const QString &key, const LastList2pages &lastMeters2pagesL);

    bool metersChanged(const LastList2pages &lastMeters2pages, const LastList2pages &lastMeters2pagesL);

    void onElectricitylistOfMeters(const UniversalMeterSettList &activeMeters, const MyNi2model &switchedOffMeters, const bool &checkOffMeters);

    QStringList addRelayRow(const UniversalMeterSett &m);

    QStringList addDateTimeRow(const UniversalMeterSett &m);

    QStringList addWaterProfileRow(const UniversalMeterSett &m);


    MyListStringList getRowsList(QMap<QString, QStringList> &mapPage, const QStringList &listNiNotchanged, const QMap<QString, QStringList> &mapPageL, const QStringList listNI, const int &rowsCounter);


    void createLocalSocketObject();

    void createPeredavatorEmbeeManager();



    struct SaveLaterMeters
    {
        quint8 meterType;
        QVariantList lastMeterList;
        SaveLaterMeters() {}
    } lastSaveMeterList;


    QMap<QString, QStringList> relayPage;
    QMap<QString, QStringList> dateTimePage;
    QMap<QString, QStringList> waterSchedulerPage;

    QStringList liststat;

//    QVariantHash lastWaterSleepProfile;
    QMap<QString, QString> mapProfLine2profName;

    int lastPageMode;//пам'ятає яка сторінка запустила опитування, щоб при скасуванні завдання відправити відповідну команду
    bool pageModeUpdated;

    bool isDbReady4read;
};

#endif // ZBYRMETERLISTMEDIUM_H
