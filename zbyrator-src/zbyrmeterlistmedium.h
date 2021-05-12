#ifndef ZBYRMETERLISTMEDIUM_H
#define ZBYRMETERLISTMEDIUM_H

///[!] widgets-shared
#include "gui-src/guiifacemedium.h"


///[!] task-types
#include "src/zbyrator-v2/zbyratordatatypehelper.h"


///[!] guisett-shared-core
#include "src/nongui/classmanagermeterinfo.h"
#include "src/ucdev/ucdevicetreewatcher.h"

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

    UCDeviceTreeWatcher *ucDeviceTreeW;

    UniversalMeterSettList lastAllMeters;

    UCDataState getTemplateValidator();

    void importGroups2metersFile();

    LastMetersStatusesManager *metersStatusManager;

    void resetVariables4pollStarted();
signals:


    void setWaterMeterSchedulerPageSett(MyListStringList listRows, QVariantMap col2data, QStringList headerH, QStringList header, bool hasHeader);





    void realoadMeters2meterManager();//if changed NI, model, SN, removed meter, added meter,


    void startTmrSaveLater();

    void onConfigChanged(quint16 command, QVariant data);

    //meterlistwdgt
    void onReloadAllMeters2zbyrator();


    void sendMeAlistOfMeters(quint8 deviceType);



    void onUpdatedSavedList(int activeMetersSize, int switchedOffMetersSize, int meterElectricityActive, int metersWaterActive);




    //to mainwindow
    void openEditMacProfileWdgt(bool isEditMode, QLineEdit *le );



//    void onAddMeters(quint8 deviceType, UniversalMeterSettList activeMeters, MyNi2model switchedOffMeters, bool checkOffMeters);//obsolete



//    void updateHashSn2meter(QHash<QString,QString> hashMeterSn2memo, QHash<QString,QString> hashMeterSn2ni, QHash<QString, QString> hashMeterNi2memo);
//    void updateHashSn2meter(QHash<QString,QString> hashMeterSn2memo, QHash<QString,QString> hashMeterSn2ni, QHash<QString, ClassManagerMeterInfo::MeterNi2info> hashMeterNi2info, QStringList listnis);
    void updateHashSn2meter(QHash<QString,QString> hashMeterSn2memo, QHash<QString,QString> hashMeterSn2ni, QHash<QString, ClassManagerMeterInfo::MeterNi2info> hashMeterNi2info, QStringList listnis, quint8 deviceType);


    void onAllMeters(UniversalMeterSettList allMeters);//it is used by start poll preparation tool

    void onStatisticChanged(QString pollCodeStr, QStringList oneRow);

    void onCOMMAND_READ_POLL_STATISTIC(QStringList list);


    void onZbyratorConfigChanged(quint16 command, QVariant dataVar);




//    void meterRelayStatus(QString ni, QDateTime dtLocal, QString stts);
    void add2fileMeterRelayStatus(QString ni, QDateTime dtLocal, quint8 mainstts, quint8 secondarystts);//2 cache


    void meterDateTimeDstStatus(QString ni, QDateTime dtLocal, QString stts);


    void reloadSavedSleepProfiles();




//Database and meterjournal
//    void data2dbMedium(quint16 command, QVariant varData);
    void stopReadDatabase();

//    void appendDataDatabase(QVariantHash hash);

//    void appendDataDatabaseMJ(QVariantHash hash);
    void setPbReadEnableDisable(bool disable);
    void setLblWaitTxtDatabase(QString s);
//    void setLblWaitTxtDatabaseMj(QString s);





    void reloadSettings();


    //zbyrator ipc
    void command2extensionClient(quint16 command, QVariant dataVar);
//
    void onAboutZigBee(QVariantHash hash);

    void relayStatusChanged(QVariantMap map);//to ZbyratorSocket

    void pbStopAnimateClick();
    void onExternalCommandProcessed();

    void killAllObjects();
//    void onElMeterRelayChanged(QVariantHash h); do not use it



//to zbyrator
    void reloadIfaceChannels();




    void startStopDirectAccessService(bool start);

    void setUCDirectAccessServiceParams(qint8 mode, quint16 defport);


    void onPollStarted(UCSelectFromDB select, QString yourSenderName, QStringList listHeaderDb, quint8 deviceType);


//to the poll tabs
    void onUCLastReceivedDeviceRecordsQuickCollectChanged(UCLastReceivedDeviceRecords records);//connect it to GUI


public slots:
    void onAllMetersSlot(UniversalMeterSettList allMeters);

    void meterElectricityModelChanged(QVariantList meters);
    void meterWaterModelChanged(QVariantList meters);

    void onSaveLater();

    void doReloadListOfMeters(quint8 deviceType);

    void onAlistOfMeters(quint8 deviceType, UniversalMeterSettList activeMeters, MyNi2model switchedOffMeters);



    void createDataCalculator();

    void createDatabaseMedium();

    //6.17
    void onAllStatHash(QStringList allstat);



    void onTaskCanceled(quint8 pollCode, QString ni, qint64 dtFinished, quint8 rez);

    void setLastPageId(QString accsblName);

//    void onReloadAllMeters();  do not uset it , it has deviceTree

    void onReloadAllZbyratorSettingsLocalSocket();

    void createPeredavatorEmbeeManagerLater();

    void command4devSlotLocalSocket(quint16 command, QString args);//pollCode args



    void mWrite2RemoteDev(quint16 command, QVariant dataVar);//from guihelper




    void updateRelayStatuses4meterlist();

    void setPbWriteDis(bool disabled);

    void command2extension(quint16 extName, quint16 command, QVariant data);





    //ucDeviceTree, It is better to add some delay before execution
    //9.31
    void onGetUCEMeterSettings(QString senderName);

    void onPutUCEMeterSettings(UCEMeterSettings settings, QString senderName);
    void onPutUCWMeterSettings(UCWMeterSettings settings, QString senderName);


    void onGetUCEMeterRelayState(QString senderName);

    void meterRelayStatus(QString ni, QDateTime dtLocal, quint8 mainstts, quint8 secondarystts); //zbyrator-bbb sends it directly, use it for canceled tasks



    //9.31, 9.61


    void onGetUCSupportedMetersInfo(QString senderName);

    void onGetUCWMeterSettings(QString senderName);


    //6.60 View tasks of the meter
    void onCommand2killMeterTasks(QList<quint32> lDecimalTaskIDs, QString senderName);

    void vanishFinishedTasks();//


    void onUCWMeterSettingsChanged(UCWMeterSettings settings);

    void waterMeterSchedulerStts(QString ni, QDateTime dtLocal, QString stts, QVariantHash sheduler, QString src);



    //5.5
    void onCommandOpenDirectAccess(UCOpenDirectAccessCommand request, QString senderName);

    void resetStopDirectAccess();

    void setVirtualMetersSett(NI2vmGSNsett vmsett);


    void sendAllMeters();



private:
    struct LastList2pages
    {
        QStringList listNI;
        QStringList mainParams; //model version SN
        LastList2pages() {}
    } ;

    QHash<QString,QString> waterNi2sn;


    void createUcDevTree();

    QMap<QString, LastList2pages> mapMeters2pages;


    UCEMeterSettingsOneRow universalMeterSett2emeterSettings(const UniversalMeterSett &m);

    UCWMeterSettingsOneRow universalMeterSett2wmeterSettings(const UniversalMeterSett &m);


    UCPollDeviceSettings universalMeterSett2baseSettings(const UniversalMeterSett &m);

    UCMeterDeviceSettings universalMeterSett2meterSettings(const UniversalMeterSett &m);




    QStringList universalMeterSett2listRow(const UniversalMeterSett &m, QStringList &ldata, QList<int> &lcols);

    QStringList universalMeterSett2listRowElectricity(const UniversalMeterSett &m);

    QStringList universalMeterSett2listRowWater(const UniversalMeterSett &m, QStringList &ldata, QList<int> &lcols);


    bool metersChanged(QMap<QString, LastList2pages> &mapMeters2pages, const QString &key, const LastList2pages &lastMeters2pagesL);

    bool metersChanged(const LastList2pages &lastMeters2pages, const LastList2pages &lastMeters2pagesL);

    void onElectricitylistOfMeters(const UniversalMeterSettList &activeMeters, const MyNi2model &switchedOffMeters, const bool &checkOffMeters);




    MyListStringList getRowsList(QMap<QString, QStringList> &mapPage, const QStringList &listNiNotchanged, const QMap<QString, QStringList> &mapPageL, const QStringList listNI, const int &rowsCounter);


    void createLocalSocketObject();

    void createPeredavatorEmbeeManager();

    void updateRelayStatuses4meterlistExt(const QMap<QString, UCEMeterRelayStateOneRelay> &relaysttsmap);


    struct SaveLaterMeters
    {
        quint8 deviceType;
        QVariantList lastMeterList;
        SaveLaterMeters() {}
    } lastSaveMeterList;




//    QVariantHash lastWaterSleepProfile;
//    QMap<QString, QString> mapProfLine2profName; do not use it

    int lastPageMode;//пам'ятає яка сторінка запустила опитування, щоб при скасуванні завдання відправити відповідну команду
    bool pageModeUpdated;

    bool isDbReady4read;

    bool pbWriteDis;


};

#endif // ZBYRMETERLISTMEDIUM_H
