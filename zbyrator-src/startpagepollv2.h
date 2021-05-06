#ifndef STARTPAGEPOLLV2_H
#define STARTPAGEPOLLV2_H

///[!] widgets-meters
#include "dataconcetrator-pgs/templates/startquickpollwdgt.h"
#include "dataconcetrator-pgs/templates/waterstartpollsleepdlg.h"


///[!] quick-collect
#include "zbyrator-src/zbyrmeterlistmedium.h"


//StartExchange Quick Collect use this as end class to show to users

class StartPagePollV2 : public StartQuickPollWdgt
{
    Q_OBJECT
public:
    explicit StartPagePollV2(GuiHelper *gHelper, QWidget *parent = nullptr);

    ZbyrMeterListMedium *metersListMedium;


    QStringList getEnrgList4code(const quint8 &code);


    //main tab section


    MyPollCodeList getLvIconsAndTexts(const int &version);//device types



    void createTab(const StartPollTabSettExt &sett);

    bool canContinueWithTheseSettings(const StartPollTabSettExt &sett);

    bool createObjectsForPollAllMetersMode(const StartPollTabSettExt &selsett, QString &mess);


signals:
    void onCbxIgnoreRetr(bool ischecked);

    void onCbxOnlyGlobalConnection(bool ischecked);

    void requestToSwitchIgnoreCycles(bool enable);




    void killSelectMeters4poll();

    void onPollStarted(quint8 pollCode, QStringList listEnrg, QString dateMask, int dotPos, bool allowDate2utc);

    void addWdgt2stackWdgt(QWidget *w, const int &wdgtType, bool oneShot, QString actTxt, QString actIco);


    void killCurrentTask();



    void command4dev(quint16 command, QVariantMap args);//pollCode args

public slots:
    void disconnectMeFromAppendData();





    void command4devSlot(quint16 command, QVariantMap map);//pollCode args

    void onSelectMeters4pollKickedOff();

    void onCurrentProcessingTabKilledSlot();


private:

    void addQuickPollPanel();




};

#endif // STARTPAGEPOLLV2_H
