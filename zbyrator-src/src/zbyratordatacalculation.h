#ifndef ZBYRATORDATACALCULATION_H
#define ZBYRATORDATACALCULATION_H


///[!] parametryzator-base
#include "gui-db-shared-src/zbyratordatacalculationbase.h"


#include "ucdevicetreetypes.h"


class ZbyratorDataCalculation : public ZbyratorDataCalculationBase
{
    Q_OBJECT
public:
    explicit ZbyratorDataCalculation(QObject *parent = nullptr);


//do not process the data here
//    QList<int> getSpecialPollCodes();

//    void processSpecialPollCode(const int &code, int &insertSnNiCorrection, QHash<QString,QString> &strhash);

    QStringList getHeader4specialPollCode(const int &code);

    QMap<quint8, QHash<QString,QString> > deviceType2ni2sn;

    QString gimmeSn4thisNI(const QString &ni);

    QString gimmeSn4thisNIbyDevType(const QString &ni, const quint8 &deviceType);

signals:



public slots:

    void onMeterPollCancelled(QString ni, QString stts, qint64 msec);

    void appendMeterData(QString ni, QString sn, MyListHashString data);



    void onUCEMeterSettingsChanged(UCEMeterSettings settings);

    void onUCWMeterSettingsChanged(UCWMeterSettings settings);


private:
    void setThesePollDevs(const QList<UCPollDeviceSettings> &pollDevl, const quint8 &deviceType);

    void removeTheseDeviceType(const quint8 &deviceType);





};

#endif // ZBYRATORDATACALCULATION_H
