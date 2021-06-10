#include "zbyratordatabasemedium.h"
#include "zbyratordatacalculation.h"

///[!] matilda-base
#include "matilda-bbb-src/shared/pathsresolver.h"

///[!] zbyrator-shared
#include "src/meter/meterstatehelper.h"


#include "definedpollcodes.h"

ZbyratorDatabaseMedium::ZbyratorDatabaseMedium(UCDeviceTreeWatcher *ucDeviceTreeW, QObject *parent) :
    ZbyratorDatabaseMediumBase(ucDeviceTreeW, PathsResolver::defSqliteMediumLocalServerName(), parent)
{

}

void ZbyratorDatabaseMedium::connecSpecialHeaderSignalSLot()
{
    connect(cManager4zbyrator, &ClassManager4zbyrator::gimmeTheSpecialHeader, this, &ZbyratorDatabaseMedium::gimmeTheSpecialHeader);
    connect(this, &ZbyratorDatabaseMedium::setTheSpecialHeader, cManager4zbyrator, &ClassManager4zbyrator::setTheSpecialHeader);
}

void ZbyratorDatabaseMedium::gimmeTheSpecialHeader(quint16 pollCode)
{
    QStringList header;
    switch(pollCode){

    case POLL_CODE_READ_METER_STATE: header = MeterStateHelper::getMeterStateHeaderKeys(false); break;

    case POLL_CODE_WTR_METER_STATE: header = MeterStateHelper::getWaterMeterStateHeaderKeys(false); break;

    case POLL_CODE_GAS_METER_STATE: header = MeterStateHelper::getGasMeterStateHeaderKeys(false); break;
    case POLL_CODE_PLSS_METER_STATE: header = MeterStateHelper::getPulseMeterStateHeaderKeys(false); break;

    }

    if(header.isEmpty()){
        qDebug() << "ZbyratorDatabaseMedium::gimmeTheSpecialHeader pollCode" << int(pollCode);
                    return;
    }
    emit setTheSpecialHeader(pollCode, header);
}




//void ZbyratorDatabaseMedium::onAlistOfMeters(quint8 deviceType, UniversalMeterSettList activeMeters, MyNi2model switchedOffMeters, bool checkOffMeters)
//{

//    ZbyratorDataCalculation().onAddlistOfMeters2cache(shrdObj, activeMeters, switchedOffMeters, checkOffMeters, deviceType);
//}

//void ZbyratorDatabaseMedium::setVirtualMetersSett(NI2vmGSNsett vmsett)
//{
//    ZbyratorDataCalculation().setVirtualMetersSettExt(shrdObj, vmsett);

//}
