#ifndef ZBYRATORDATACALCULATION_H
#define ZBYRATORDATACALCULATION_H

#include <QObject>
#include "src/matilda-conf/classmanagersharedobjects.h"
#include "src/zbyrator-v2/zbyratordatatypehelper.h"
#include "src/matilda/classmanagertypes.h"

class ZbyratorDataCalculation : public QObject
{
    Q_OBJECT
public:
    explicit ZbyratorDataCalculation(QObject *parent = nullptr);
    ClassManagerSharedObjects *shrdObjElectricity;
    ClassManagerSharedObjects *shrdObjWater;


    QMap<QString, UniverslaMeterOnlyCache > ni2cachedEnrg;

signals:
    void appendData2model(QVariantHash h);

    void uploadProgress(int val, QString txt);

    void setLblWaitTxt(QString s);

    void updateHashSn2meter(QHash<QString,QString> hashMeterSn2memo, QHash<QString,QString> hashMeterSn2ni, QHash<QString, QString> hashMeterNi2memo);


    void setCOMMAND_READ_POLL_STATISTIC(QVariantHash h);

public slots:
    void onThreadStarted();

    void onAlistOfMeters(quint8 meterType, UniversalMeterSettList activeMeters, MyNi2model switchedOffMeters, bool checkOffMeters);

    void appendMeterData(QString ni, QString sn, MyListHashString data);

    void onPollStarted(quint8 pollCode, QStringList listEnrg, QString dateMask, int dotPos, bool allowDate2utc);

    void onUconStartPoll(QStringList nis, quint8 meterType);

    void uploadProgressSlot(int val, QString txt);

    void onCOMMAND_READ_POLL_STATISTIC(QStringList list);

    void onMeterPollCancelled(QString ni, QString stts, qint64 msec);

private:
    void onAddlistOfMeters2cache(ClassManagerSharedObjects *shrdObj, const UniversalMeterSettList &activeMeters, const MyNi2model &switchedOffMeters, const bool &checkOffMeters);

    QStringList listEnrg;
    quint8 lastPollCode;
    bool sendHeader;
    QString lastDateMask, lastFullDateTimeMask;
    int dotPos;
    bool allowDate2utc;

    //    gHelper->hashMeterSn2ni;
    QHash<QString,QString> hashMeterSn2ni;
    QHash<QString,QString> hashMeterNi2memo;
    QHash<QString,QString> hashMeterSn2memo;

};

#endif // ZBYRATORDATACALCULATION_H
