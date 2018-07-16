#ifndef ZBYRMETERLISTMEDIUM_H
#define ZBYRMETERLISTMEDIUM_H

#include <QObject>
#include "src/zbyrator-v2/zbyratordatatypehelper.h"
#include "src/matilda/classmanagertypes.h"

class ZbyrMeterListMedium : public QObject
{
    Q_OBJECT
public:
    explicit ZbyrMeterListMedium(QObject *parent = nullptr);

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

public slots:
    void onAllMeters(UniversalMeterSettList allMeters);

    void meterModelChanged(QVariantList meters);

    void onSaveLater();

    void doReloadListOfElectricityMeters();

    void doReloadListOfMeters(quint8 meterType);

    void onAlistOfMeters(quint8 meterType, UniversalMeterSettList activeMeters, MyNi2model switchedOffMeters);

    void sendMeIfaceSett();

    void setNewSettings(QVariantHash h);

    void command4devSlot(quint16 command, QString args);//pollCode args

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


    QVariantList lastMeterList;



    QMap<QString, QStringList> relayPage;
    QMap<QString, QStringList> dateTimePage;



};

#endif // ZBYRMETERLISTMEDIUM_H
