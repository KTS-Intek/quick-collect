#include "zbyratordatacalculation.h"
#include "myucdevicetypes.h"

//#include "src/matilda/moji_defy.h"

///[!] guisett-shared
#include "src/nongui/classmanagerhelper.h"
//#include "src/nongui/showmesshelpercore.h"


///[!] zbyrator-shared
#include "src/meter/meterstatehelper.h"
#include "src/zbyrator-v2/watermeterhelper.h"


///[!] guisett-shared-core
#include "src/nongui/classmanagerdatabasereader.h"


///[!] meters-shared
//#include "zbyrator-water/src/watersleepschedulesaver.h"
#include "zbyrator-src/protocol5togui.h"


#include <QJsonObject>
#include <QJsonDocument>

#include "definedpollcodes.h"

ZbyratorDataCalculation::ZbyratorDataCalculation(QObject *parent) : ZbyratorDataCalculationBase(parent)
{

}

void ZbyratorDataCalculation::onAddlistOfMeters2cache(ClassManagerSharedObjects *shrdObj, const UniversalMeterSettList &activeMeters, const MyNi2model &switchedOffMeters, const bool &checkOffMeters, const quint8 &deviceType)
{
    Q_UNUSED(switchedOffMeters);

    if(checkOffMeters){
        switch (deviceType) {
        case UC_METER_ELECTRICITY: shrdObj->electricityMeter.clear(); break;
        case UC_METER_WATER      : shrdObj->waterMeter.clear(); break;
        }
        const QHash<int, QVariantHash> hashMemoWrite = shrdObj->hashMemoWrite;
        shrdObj->clearWrite();
        shrdObj->hashMemoWrite = hashMemoWrite;
    }
    //    gHelper->hashMeterSn2ni;
//        QHash<QString,QString> hashMeterSn2ni;
//        QHash<QString,QString> hashMeterNi2memo;

    switch (deviceType) {
    case UC_METER_ELECTRICITY: shrdObj->electricityMeter.listnis.clear(); shrdObj->electricityMeter.hashMeterNi2info.clear(); break;
    case UC_METER_WATER      : shrdObj->waterMeter.listnis.clear(); shrdObj->waterMeter.hashMeterNi2info.clear(); break;
    }


    for(int i = 0, imax = activeMeters.size(); i < imax; i++){
        const UniversalMeterSett m = activeMeters.at(i);

        if(!m.ni.isEmpty())
            ni2cachedEnrg.insert(m.ni, m.cache);



        switch (deviceType) {
        case UC_METER_ELECTRICITY:{
            shrdObj->electricityMeter.addMeter2meterNi2info(m.ni, m.memo, m.sn, m.coordinate, m.tariff, m.transformer, m.model, m.version, m.powerin, m.pollEnbl, 0, 0);
            shrdObj->electricityMeter.addMeter2meterSn2info(m.sn, m.memo, m.ni, m.coordinate, m.tariff, m.transformer, m.model, m.version, m.powerin, m.pollEnbl, 0, 0);
            break;}
        case UC_METER_WATER      :{
            shrdObj->waterMeter.addMeter2meterNi2info(m.ni, m.memo, m.sn, m.coordinate, m.tariff, m.transformer, m.model, m.version, m.powerin, m.pollEnbl, 0, 0);
            shrdObj->waterMeter.addMeter2meterSn2info(m.sn, m.memo, m.ni, m.coordinate, m.tariff, m.transformer, m.model, m.version, m.powerin, m.pollEnbl, 0, 0);
            break;}
        }




        if(!m.sn.isEmpty()){
            hashMeterSn2ni.insert(m.sn, m.ni);
            hashMeterSn2memo.insert(m.sn, m.memo);
        }
//        hashMeterNi2memo.insert(m.ni, m.memo);
    }

    QStringList listnis;
    QHash<QString, ClassManagerMeterInfo::MeterNi2info> hashMeterNi2info;


    switch (deviceType) {
    case UC_METER_ELECTRICITY:{
        listnis = shrdObj->electricityMeter.listnis;
        hashMeterNi2info = shrdObj->electricityMeter.hashMeterNi2info;
        break;}
    case UC_METER_WATER      :{
        listnis = shrdObj->waterMeter.listnis;
        hashMeterNi2info = shrdObj->waterMeter.hashMeterNi2info;
        break;}
    }

    if(!listnis.isEmpty())
        emit updateHashSn2meter(hashMeterSn2memo, hashMeterSn2ni, hashMeterNi2info, listnis, deviceType);
}

QList<int> ZbyratorDataCalculation::getSpecialPollCodes()
{
    return QList<int>() << POLL_CODE_READ_METER_STATE << POLL_CODE_WTR_METER_STATE;

}

void ZbyratorDataCalculation::processSpecialPollCode(const int &code, int &insertSnNiCorrection, QHash<QString, QString> &strhash)
{
    if(code == POLL_CODE_WTR_METER_STATE){


        const QStringList vls = strhash.value("vls").split(" ", QString::SkipEmptyParts);
        if(vls.size() > 3){
            strhash.insert("meter_bat", vls.at(0));
            strhash.insert("modem_bat", vls.at(1));
            strhash.insert("meter_bat_v", vls.at(2));
            strhash.insert("modem_bat_v", vls.at(3));

            const QVariantHash sheduler = MeterStateHelper::waterPrmsLine2hash(strhash.value("prm"));


            const QString profilelinejson = QString(QJsonDocument(QJsonObject::fromVariantHash(sheduler)).toJson(QJsonDocument::Compact));
            strhash.insert("profile_params", profilelinejson);

        }


    }else{
         if(code == POLL_CODE_READ_METER_STATE){
             const QStringList vls = strhash.value("vls").split(" ");
             if(vls.size() > 4){///home/hello_zb/My_Prog/Qt5_progs/TESTS/Matilda-units/gui/guisett-shared-core/src/nongui/classmanagerhelper.h
                 QVariantList listOneMeter;
//                 ClassManagerHelper::addVls2listPretty(listOneMeter, strhash.value("vls"), shrdObj->dateMask);

                 const int fromindx = MeterStateHelper::getMeterStateHeaderKeys(false).indexOf("balance");
                 const QStringList lk = MeterStateHelper::getMeterStateHeaderKeys(false).mid(fromindx);//                             QString("balance last_charge balance_charge balance_counter tarif_values");

                 for(int j = 0, jmax = lk.size(), jmax2 = listOneMeter.size(); j < jmax && j < jmax2; j++)
                     strhash.insert(lk.at(j), listOneMeter.at(j).toString());

             }
             strhash.insert("meter_model", strhash.take("model"));
             strhash.insert("params", strhash.value("prm"));
             insertSnNiCorrection = 1;

         }
    }

}

QStringList ZbyratorDataCalculation::getHeader4specialPollCode(const int &code)
{
    QStringList columnList;
    columnList = (code == POLL_CODE_READ_METER_STATE) ? MeterStateHelper::getMeterStateHeaderKeys(false) : MeterStateHelper::getWaterMeterStateHeaderKeys(false);
//            columnList.removeFirst();
    columnList.replace(0, "msec");
    return columnList;
}



void ZbyratorDataCalculation::onAlistOfMeters(quint8 deviceType, UniversalMeterSettList activeMeters, MyNi2model switchedOffMeters, bool checkOffMeters)
{
//    ClassManagerSharedObjects *shrdObj = 0;

//    switch (deviceType) {
//    case UC_METER_WATER         : shrdObj = shrdObjWater        ; break;
//    case UC_METER_ELECTRICITY   : shrdObj = shrdObjElectricity  ; break;
//    }

//    if(shrdObj)
    onAddlistOfMeters2cache(shrdObj, activeMeters, switchedOffMeters, checkOffMeters, deviceType);
}

void ZbyratorDataCalculation::setVirtualMetersSett(NI2vmGSNsett vmsett)
{

//    if(!listnis.isEmpty())
//        emit updateHashSn2meter(hashMeterSn2memo, hashMeterSn2ni, hashMeterNi2info, listnis, deviceType);

    bool updatesharedsettings = false;
    const QList<QString> lk = vmsett.keys();
    for(int i = 0, imax = lk.size(); i < imax; i++){
        const QString ni = lk.at(i);

        QString snPrefix = shrdObj->electricityMeter.hashMeterNi2info.value(ni).sn;
        if(snPrefix.isEmpty())
            snPrefix = ni.rightJustified(4, '0');


        const ClassManagerMeterInfo::MeterInfo ininfo = shrdObj->electricityMeter.hashMeterNi2info.value(ni).minfo;


        const ZVirtualMeters vmeters = vmsett.value(ni);
        const QList<QString> vmetersgsn = vmeters.keys();


          for(int j = 0, jmax = vmetersgsn.size(); j < jmax; j++){

              const QString GSN = vmetersgsn.at(j);


              const QString SN = snPrefix + GSN;

              if(!updatesharedsettings)
                  updatesharedsettings =  true;
              ClassManagerMeterInfo::MeterInfo info = ininfo;

              info.memo = vmeters.value(GSN).memo;


              shrdObj->electricityMeter.addMeter2meterSn2info(SN, ni, info, MeterTransformer());

              hashMeterSn2memo.insert(SN, info.memo);
          }

    }

    if(updatesharedsettings)
        emit updateHashSn2meter(hashMeterSn2memo, hashMeterSn2ni, shrdObj->electricityMeter.hashMeterNi2info, shrdObj->electricityMeter.listnis, UC_METER_ELECTRICITY);

}

void ZbyratorDataCalculation::setVirtualMetersSettExt(ClassManagerSharedObjects *shrdObj, NI2vmGSNsett vmsett)
{
    this->shrdObj = shrdObj;
    setVirtualMetersSett(vmsett);

}



void ZbyratorDataCalculation::onMeterPollCancelled(QString ni, QString stts, qint64 msec)
{
//    ClassManagerSharedObjects *shrdObj = shrdObjElectricity;
//    const QString sn = shrdObjElectricity->electricityMeter.hashMeterNi2info.value(ni).sn ;
    const QString sn = shrdObj->electricityMeter.hashMeterNi2info.value(ni).sn ;//water?

    MyListHashString listHash;

    const QDateTime dt = QDateTime::fromMSecsSinceEpoch(msec);

    qint64 msecMinus = 0;
    switch(lastPollCode){
    case POLL_CODE_WTR_END_DAY      :
    case POLL_CODE_WTR_END_MONTH    :

    case POLL_CODE_READ_END_DAY     :
    case POLL_CODE_READ_END_MONTH   : msecMinus = (qint64)dt.toLocalTime().offsetFromUtc() * 1000;
    }
    QHash<QString,QString> h;
    h.insert("msec", QString::number(dt.toUTC().toMSecsSinceEpoch() - msecMinus));
    h.insert("stts", stts);

    listHash.append(h);

    appendMeterData(ni, sn, listHash);
}




