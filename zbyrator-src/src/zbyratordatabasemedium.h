#ifndef ZBYRATORDATABASEMEDIUM_H
#define ZBYRATORDATABASEMEDIUM_H


///[!] parametryzator-base
#include "gui-db-shared-src/zbyratordatabasemediumbase.h"


class ZbyratorDatabaseMedium : public ZbyratorDatabaseMediumBase
{
    Q_OBJECT
public:
    explicit ZbyratorDatabaseMedium(QObject *parent = nullptr);

public slots:

    void onAlistOfMeters(quint8 deviceType, UniversalMeterSettList activeMeters, MyNi2model switchedOffMeters, bool checkOffMeters);

    void setVirtualMetersSett(NI2vmGSNsett vmsett);
};

#endif // ZBYRATORDATABASEMEDIUM_H
