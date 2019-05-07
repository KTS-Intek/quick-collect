#ifndef STARTPAGEPOLLV2_H
#define STARTPAGEPOLLV2_H

///[!] widgets-meters
#include "dataconcetrator-pgs/selectfromdatabasetemplate.h"


///[!] quick-collect
#include "zbyrator-src/zbyrmeterlistmedium.h"
#include "zbyrator-src/waterstartpollsleepdlg.h"

class StartPagePollV2 : public SelectFromDatabaseTemplate
{
    Q_OBJECT
public:
    explicit StartPagePollV2(GuiHelper *gHelper, QWidget *parent = nullptr);
    StartPollTabSettExt lastSelsett;

    ZbyrMeterListMedium *metersListMedium;

    QString getArgsElectricMeter(const QStringList &nis, const QDateTime &dtTo, const QDateTime &dtFrom, QString &mess);

    QString getArgsWaterMeter(const QStringList &nis, const QDateTime &dtTo, const QDateTime &dtFrom, QString &mess);

    QString getOneLineModelNiPas(const quint8 &meterType, const QStringList &nis);

    QStringList getEnrgList4code(const quint8 &code);


    //main tab section
    QString getTabTitle();

    QString getTabIconPath();

    LvIconTextCommandList getLvIconsAndTexts(const int &version);//device types

    OneDevicePollCodes getDeviceSelectSett4adev(const int &devtype);

    QVariant getPageSett4read(bool &ok, QString &mess);




signals:

    void onReloadAllMeters();


    void killSelectMeters4poll();

    void onPollStarted(quint8 pollCode, QStringList listEnrg, QString dateMask, int dotPos, bool allowDate2utc);

    void addWdgt2stackWdgt(QWidget *w, const int &wdgtType, bool oneShot, QString actTxt, QString actIco);


    void lockPbRead(bool disable); //to startexchange


    void lockButtons(bool disable);//from StartExchange

    void killCurrentTask();
public slots:
    void disconnectMeFromAppendData();

    void onUpdatedSavedList(int activeMetersSize, int switchedOffMetersSize, int meterElectricityActive, int metersWaterActive);

    void getMetersLater();

    void onPbReadClicked();

    void setWaterSleepSett(bool sendSleepCommand, int secs, bool checkProfile);

    void setNi2model(QStringList modelnis);

    void command4devSlot(quint16 command, QVariantMap map);//pollCode args

    void onSelectMeters4pollKickedOff();

    void onCurrentProcessingTabKilledSlot();
    void onLockButtons(bool disable);//from StartExchange

private:
    bool startPollOneMeterMode(const StartPollTabSettExt &selsett, QString &mess);

    bool startPollAllMetersMode(const StartPollTabSettExt &selsett, QString &mess);

    void createTab(const StartPollTabSettExt &sett);

    void openWtrSleepDlg();

    WaterStartPollSleepDlg::WtrSleepPollSett lastWtrSett;
    struct TempStartPollLastSett
    {
        QString lastLineModelNiPasswd;
        bool isSelectMeters4pollProcessing;
        QString lastWdgtActive;
        TempStartPollLastSett() : isSelectMeters4pollProcessing(false){}
    } lTempPollSett;

};

#endif // STARTPAGEPOLLV2_H
