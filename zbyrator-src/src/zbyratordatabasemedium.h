#ifndef ZBYRATORDATABASEMEDIUM_H
#define ZBYRATORDATABASEMEDIUM_H


///[!] parametryzator-base
#include "gui-db-shared-src/zbyratordatabasemediumbase.h"


class ZbyratorDatabaseMedium : public ZbyratorDatabaseMediumBase
{
    Q_OBJECT
public:
    explicit ZbyratorDatabaseMedium(UCDeviceTreeWatcher *ucDeviceTreeW, QObject *parent = nullptr);

    void connecSpecialHeaderSignalSLot();

signals:
    void setTheSpecialHeader(quint16 pollCode, QStringList header);//to cManager


public slots:
    void gimmeTheSpecialHeader(quint16 pollCode);//from cManager


//    void onAlistOfMeters(quint8 deviceType, UniversalMeterSettList activeMeters, MyNi2model switchedOffMeters, bool checkOffMeters);

//    void setVirtualMetersSett(NI2vmGSNsett vmsett);
};

#endif // ZBYRATORDATABASEMEDIUM_H
