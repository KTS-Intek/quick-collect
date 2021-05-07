#ifndef ZBYRATORDATACALCULATION_H
#define ZBYRATORDATACALCULATION_H


///[!] parametryzator-base
#include "gui-db-shared-src/zbyratordatacalculationbase.h"



class ZbyratorDataCalculation : public ZbyratorDataCalculationBase
{
    Q_OBJECT
public:
    explicit ZbyratorDataCalculation(QObject *parent = nullptr);

    void onAddlistOfMeters2cache(ClassManagerSharedObjects *shrdObj, const UniversalMeterSettList &activeMeters, const MyNi2model &switchedOffMeters, const bool &checkOffMeters, const quint8 &deviceType);


    QList<int> getSpecialPollCodes();

    void processSpecialPollCode(const int &code, int &insertSnNiCorrection, QHash<QString,QString> &strhash);

    QStringList getHeader4specialPollCode(const int &code);



signals:



public slots:

    void onAlistOfMeters(quint8 deviceType, UniversalMeterSettList activeMeters, MyNi2model switchedOffMeters, bool checkOffMeters);

    void setVirtualMetersSett(NI2vmGSNsett vmsett);

    void setVirtualMetersSettExt(ClassManagerSharedObjects *shrdObj, NI2vmGSNsett vmsett);






    void onMeterPollCancelled(QString ni, QString stts, qint64 msec);



private:



};

#endif // ZBYRATORDATACALCULATION_H
