#include "zbyratordatabasemedium.h"
#include "zbyratordatacalculation.h"

///[!] matilda-base
#include "matilda-bbb-src/shared/pathsresolver.h"

ZbyratorDatabaseMedium::ZbyratorDatabaseMedium(QObject *parent) : ZbyratorDatabaseMediumBase(PathsResolver::defSqliteMediumLocalServerName(), parent)
{

}




void ZbyratorDatabaseMedium::onAlistOfMeters(quint8 deviceType, UniversalMeterSettList activeMeters, MyNi2model switchedOffMeters, bool checkOffMeters)
{

    ZbyratorDataCalculation().onAddlistOfMeters2cache(shrdObj, activeMeters, switchedOffMeters, checkOffMeters, deviceType);
}

void ZbyratorDatabaseMedium::setVirtualMetersSett(NI2vmGSNsett vmsett)
{
    ZbyratorDataCalculation().setVirtualMetersSettExt(shrdObj, vmsett);

}
