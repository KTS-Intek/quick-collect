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

signals:
    void appendData2model(QVariantHash h);


public slots:
    void onThreadStarted();

    void onAlistOfMeters(quint8 meterType, UniversalMeterSettList activeMeters, MyNi2model switchedOffMeters, bool checkOffMeters);

    void appendMeterData(QString ni, QString sn, MyListHashString data);

    void onPollStarted(quint8 pollCode, QStringList listEnrg, QString dateMask, bool allowDate2utc);

private:
    void onAddlistOfMeters2cache(ClassManagerSharedObjects *shrdObj, const UniversalMeterSettList &activeMeters, const MyNi2model &switchedOffMeters, const bool &checkOffMeters);

    QStringList listEnrg;
    quint8 lastPollCode;
    bool sendHeader;
    QString lastDateMask, lastFullDateTimeMask;

    bool allowDate2utc;
};

#endif // ZBYRATORDATACALCULATION_H
