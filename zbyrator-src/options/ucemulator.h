#ifndef UCEMULATOR_H
#define UCEMULATOR_H


///[!] matilda-conf-shared-widgets
#include "main-pgs/startdevwdgt4ucon.h"


///[!] widgets-shared
#include "template-pgs/smplptewdgt.h"


///[!] matilda-bbb-serverside-shared
#include "matilda-bbb-src/decoders/decodematildaprotocolwithjson.h"
#include "matilda-bbb-src/decoders/uconconnectionholdertypes.h"

///[!] Matilda-IO
#include "matilda-bbb-src/tcpspeedstat.h"


///[!] matilda-bbb-clientside
//#include "src/matilda/classmanagerprocessor.h"
#include "src/matilda/classmanager.h"
#include "src/matilda/matildaclient.h"


//ZbyratorOptions uses it

class UcEmulator : public StartDevWdgt4ucon
{
    Q_OBJECT

public:
    explicit UcEmulator(const bool &enableTestFeatures, GuiSett4all *gSett4all, UCDeviceTreeWatcher *ucDeviceTreeW, QWidget *parent = 0);
    ~UcEmulator();
    CachedWriteSett socketcache;
    bool enableTestFeatures;

    UCDeviceTreeWatcher *ucDeviceTreeW;

    MatildaClient *clientdecoder;


    QStringList realPageNameByDev(const int &devType, const int &protocolVersion);//en_GB

    int getDefDevType();

    int getDefProtocolVersion();

//    void createDashBoardWidget();

//    QScrollArea *getDashBoardScrollArea();

//    MatildaConfWidget *getDashBoardWidget();

    void createClassManager();

    void onNewMatildaWidgetAdded(MatildaConfWidget *w);


signals:
    void reloadAllSettings();


    //to UCommandServer
    void addThisIPToBlackList(QString);

    void removeThisIpFromBlackList(QString);

    void addError2Log(QString mess);

    void setIneedMoreTime(quint16 command, qint32 elTime, qint64 someData); // to databaseReader to reset the timer

    void command2extension(quint16 extName, quint16 command, QVariant data);


    void addData2prepaidTable(QVariantHash hDt2appendData);

    void command4dev(quint16 command, QString args);
    void reloadSErverSett();

    void reloadMacSett();

    void reloadUdpBeaconSett(bool);



    //to decoder
    void restartTimeObject();


    //local
    void startRealodSettLater();
    void reloadSettings2ucEmulator();



    void setThisSaveFileName(QString);

public slots:
    void activatePageLater();

    void data2matildaExtSlot(quint16 command, QVariant varData, int secs4loop);

    void data2matildaNextSlot(quint16 command, QVariant varData);


    void mWriteIneedMoreTime(const quint16 &command, const qint32 &elTime, const qint64 &someData);

    void disconnLater(qint64 msec);
    void mWriteToSocket(const QVariant &s_data, const quint16 &s_command);

    void onDisconnByDecoder();
    void onDisconnExt(const bool &allowdecoder);



    void saveSettings(int block, QVariantHash hash);



    void onSendThisCommandNowSlot(const quint16 &command, const QVariant &dataVar, const bool &itHasAnswer);//do not forge to connect to signal

private slots:
    void activatePage();

    void createDecoderLater();

    void createDecoder();


    void makeAuthorization();

    void mWriteLater();
private:
//    SmplPteWdgt *dashBoardW;
//    QScrollArea *dashBoardSa;

    DecodeMatildaProtocolWithJSON *decoder;

    struct MyWriteLater
    {
        QList<quint16> command;
        QVariantList varData;
        QList<bool> itHasAnswer;

        MyWriteLater() {}
    } writelater;

//    ClassManagerProcessor *manager;

    ClassManager *cManager;

};

#endif // UCEMULATOR_H
