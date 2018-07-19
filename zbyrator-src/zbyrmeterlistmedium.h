#ifndef ZBYRMETERLISTMEDIUM_H
#define ZBYRMETERLISTMEDIUM_H

#include <QObject>
#include <QLineEdit>
#include "src/zbyrator-v2/zbyratordatatypehelper.h"
#include "src/matilda/classmanagertypes.h"
#include "zbyrator-src/src/ifacesettloader.h"

class ZbyrMeterListMedium : public QObject
{
    Q_OBJECT
public:
    explicit ZbyrMeterListMedium(QObject *parent = nullptr);

    IfaceSettLoader *ifaceLoader;

    QVariantHash getActiveIfaceSett() const;

    QVariantHash getIfaceSett() const;

signals:
    void setMeterListPageSett(MyListStringList listRows, QVariantMap col2data, QStringList headerH, QStringList header, bool hasHeader);

    void setRelayPageSett(MyListStringList listRows, QVariantMap col2data, QStringList headerH, QStringList header, bool hasHeader);

    void setDateTimePageSett(MyListStringList listRows, QVariantMap col2data, QStringList headerH, QStringList header, bool hasHeader);


    void realoadMeters2meterManager();//if changed NI, model, SN, removed meter, added meter,


    void startTmrSaveLater();

    void onConfigChanged(quint16 command, QVariant data);

    //meterlistwdgt
    void onReloadAllMeters();


    void sendMeAlistOfMeters(quint8 meterType);


    void setIfaceSett(QVariantHash h);

    void onUpdatedSavedList(int activeMetersSize, int switchedOffMetersSize);

    void command4dev(quint16 command, QString args);//pollCode args

    void setTcpClientCompliter(QStringList tcpServers);

    //to mainwindow
    void openEditMacProfileWdgt(bool isEditMode, QLineEdit *le );
    void showMess(QString s);

    void setThisIfaceSett(QVariantMap interfaceSettings);
    void setPollSaveSettings(quint16 meterRetryMax, quint16 meterRetryMaxFA, bool hardAddrsn, bool enableW4E, bool corDTallow, qint32 messCountBeforeReady, qint32 messCountAfter, qint32 corDTintrvl);


    void giveMeYourCache();//from GUI

    void ifaceLogStr(QString line); //to GUI

    void onAddMeters(quint8 meterType, UniversalMeterSettList activeMeters, MyNi2model switchedOffMeters, bool checkOffMeters);

    void appendMeterData(QString ni, QString sn, MyListHashString data);

    void appendData2model(QVariantHash h);//to tab

    void onPollStarted(quint8 pollCode, QStringList listEnrg, QString dateMask, int dotPos, bool allowDate2utc);

    void onConnectionStateChanged(bool isActive);

    void onUconStartPoll(QStringList nis, quint8 meterType);

    void setLblWaitTxt(QString s);

    void killUconTasks();

    void updateHashSn2meter(QHash<QString,QString> hashMeterSn2memo, QHash<QString,QString> hashMeterSn2ni, QHash<QString, QString> hashMeterNi2memo);


public slots:
    void onAllMeters(UniversalMeterSettList allMeters);

    void meterModelChanged(QVariantList meters);

    void onSaveLater();

    void doReloadListOfElectricityMeters();

    void doReloadListOfMeters(quint8 meterType);

    void onAlistOfMeters(quint8 meterType, UniversalMeterSettList activeMeters, MyNi2model switchedOffMeters);

    void sendMeIfaceSett();

    void openTcpServerDlg(QLineEdit *le);

    void openM2mDlg(QLineEdit *le);


    void setNewSettings(QVariantHash h);

    void command4devSlot(quint16 command, QString args);//pollCode args

    void onListChanged(const QStringList &list, const int &tag);

    void createDataCalculator();

private:
    struct LastList2pages
    {
        QStringList listNI;
        QStringList mainParams; //model version SN
        LastList2pages() {}
    } lastMeters2pages;

    void onElectricitylistOfMeters(const UniversalMeterSettList &activeMeters, const MyNi2model &switchedOffMeters, const bool &checkOffMeters);

    QStringList addRelayRow(const UniversalMeterSett &m);

    QStringList addDateTimeRow(const UniversalMeterSett &m);

    MyListStringList getRowsList(QMap<QString, QStringList> &mapPage, const QStringList &listNiNotchanged, const QMap<QString, QStringList> &mapPageL, const QStringList listNI, const int &rowsCounter);

    void openIpHistoryDlg(const int &dlgMode, QLineEdit *le);


    bool updateInterfaceSettings();

    QVariantList lastMeterList;



    QMap<QString, QStringList> relayPage;
    QMap<QString, QStringList> dateTimePage;



};

#endif // ZBYRMETERLISTMEDIUM_H
