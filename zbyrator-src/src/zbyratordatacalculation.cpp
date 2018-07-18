#include "zbyratordatacalculation.h"
#include "src/zbyrator-v2/myucmmeterstypes.h"
#include "src/matilda/showmesshelper.h"
#include "src/matilda/moji_defy.h"
#include "src/matilda/classmanagerhelper.h"

ZbyratorDataCalculation::ZbyratorDataCalculation(QObject *parent) : QObject(parent)
{

}

void ZbyratorDataCalculation::onThreadStarted()
{
    shrdObjElectricity = new ClassManagerSharedObjects(this);
    shrdObjWater = new ClassManagerSharedObjects(this);

}

void ZbyratorDataCalculation::onAlistOfMeters(quint8 meterType, UniversalMeterSettList activeMeters, MyNi2model switchedOffMeters, bool checkOffMeters)
{
    ClassManagerSharedObjects *shrdObj = 0;

    switch (meterType) {
    case UC_METER_WATER         : shrdObj = shrdObjWater        ; break;
    case UC_METER_ELECTRICITY   : shrdObj = shrdObjElectricity  ; break;
    }

    if(shrdObj)
        onAddlistOfMeters2cache(shrdObj, activeMeters, switchedOffMeters, checkOffMeters);
}

void ZbyratorDataCalculation::appendMeterData(QString ni, QString sn, MyListHashString data)
{
    ClassManagerSharedObjects *shrdObj = shrdObjElectricity;
    QVariantHash h;

    bool dateInUtc = (lastPollCode == POLL_CODE_READ_VOLTAGE || lastPollCode == POLL_CODE_READ_TOTAL || lastPollCode == POLL_CODE_READ_POWER || lastPollCode == POLL_CODE_METER_STATUS);


    h.insert("itIsGeliks", (lastPollCode == POLL_CODE_READ_VOLTAGE || lastPollCode == POLL_CODE_READ_TOTAL));
    h.insert("shrdObj->dateMask", lastDateMask);
    h.insert("hasHeader", false);
    h.insert("dateInUtc", allowDate2utc);

    bool lastPairSn2meterInfoChanged = false;

    if(sendHeader){
        sendHeader = false;
        QStringList lHeader, lHeaderData;

        shrdObj->lastPairSn2meterInfo.clear();
        lastPairSn2meterInfoChanged = true;

        QStringList columnList = QString("msec meter_sn meter_ni memo").split(" ");
        columnList.append(listEnrg);
        columnList.append("stts");

        shrdObj->lastHeaderData = columnList;

        const QHash<QString,QString> hKeyHuman = ShowMessHelper::columnKey2humanLang();

        shrdObj->lastMemoPos = columnList.indexOf("memo");//має бути справа від NI


        shrdObj->lastSnIndx = columnList.indexOf("meter_sn");
        shrdObj->lastNiIndx = columnList.indexOf("meter_ni");
        shrdObj->lastDateIsMsec = columnList.contains("msec");

        for(int i = 0, iMax = columnList.size(); i < iMax; i++){
            lHeader.append(hKeyHuman.value(columnList.at(i), columnList.at(i)));
            lHeaderData.append(columnList.at(i));
        }
        shrdObj->lastColumnListSize = lHeader.size();
        shrdObj->lastColumnListSize--;

        h.insert("hasHeader", true);
        h.insert("lHeader", lHeader);
        h.insert("lHeaderData", lHeaderData);

        shrdObj->lastPollCode = lastPollCode;
        shrdObj->lastDateIsMsec = true;
    }


    if(data.isEmpty()){
        QHash<QString,QString> hh;
        hh.insert("stts", tr("no data"));
        data.append(hh);
    }

    QVariantList meters;
    QVariantHash hashRowCol2varData;

    //        const Data2listInputSett d2linput(meterSnIndx, niIndx, itIsGeliks, dateTimeMask, colFrom, itIsGeliks ? 1 : 2);
    int colFrom = 4;// shrdObj->lastDateIsMsec ? 4 : 3;//model before SN & NI

    const Data2listInputSett d2linput(shrdObj->lastSnIndx, shrdObj->lastNiIndx, true, lastDateMask, 4, 1);//
    hashRowCol2varData.insert("colFrom", colFrom);

    const int columnListSize = shrdObj->lastColumnListSize;//(itIsGeliks) ? shrdObj->lastColumnListSize : (shrdObj->lastColumnListSize - 1);
    int rowCounter = 0;

//    const QStringList leftList = QString("msec meter_sn meter_ni memo").split(" ", QString::SkipEmptyParts);
//    const int leftListSize = leftList.size();

    for(int i = 0, imax = data.size(), emax = listEnrg.size(); i < imax; i++){
        const QHash<QString,QString> row = data.at(i);

        QStringList list;
//        for(int j = 0; j < leftListSize; j++)
//            list.append(row.value(leftList.at(j)));
        list.append(row.value("msec"));
        list.append(sn);
        list.append(ni);//the next will be memo

        for(int j = 0; j < emax; j++)
            list.append(row.value(listEnrg.at(j)));

        list.append(row.value("stts"));

        QVariantList listOneMeter;

        const Data2listOutSett outsett = (shrdObj->lastPollCode == POLL_CODE_READ_METER_STATE) ?
                    ClassManagerHelper::addData2listState(listOneMeter, shrdObj, hashRowCol2varData, list, columnListSize, d2linput, rowCounter) :
                    ClassManagerHelper::addData2list(listOneMeter, shrdObj, hashRowCol2varData, list, columnListSize, d2linput, rowCounter);

        if(!listOneMeter.isEmpty()){
            meters.append(QVariant(listOneMeter));
            QVariantHash h = shrdObj->lastPairSn2meterInfo.value(outsett.meterSN).toHash();
            lastPairSn2meterInfoChanged = true;

            quint64 oneMeterCounter = h.value("counter", 0).toULongLong();
            oneMeterCounter++;
            if(shrdObj->lastSnIndx > 0 && !outsett.meterSN.isEmpty() && !shrdObj->lastPairSn2meterInfo.contains(outsett.meterSN)){
                h.insert("NI", outsett.meterNi);
                h.insert("memo", shrdObj->meterMemoFromCache(outsett.meterSN, outsett.meterNi));
            }
            h.insert("counter", oneMeterCounter);
            shrdObj->lastPairSn2meterInfo.insert(outsett.meterSN, h);

            rowCounter++;
        }

    }

    if(lastPairSn2meterInfoChanged)
        hashRowCol2varData.insert("\r\nlastPairSn2meterInfo\r\n", shrdObj->lastPairSn2meterInfo);


    if(!h.isEmpty() || !meters.isEmpty()){
        h.insert("meters", meters);
        h.insert("iHash", hashRowCol2varData);
        h.insert("dateInUtc",dateInUtc );

        emit appendData2model(h);
    }


//    h.insert("msec", QString::number(dt.toUTC().toMSecsSinceEpoch()));
//    h.insert("stts", tr("ok)"));
    //model for MEter Journal

}

void ZbyratorDataCalculation::onPollStarted(quint8 pollCode, QStringList listEnrg, QString dateMask, bool allowDate2utc)
{
    sendHeader = true;
    lastPollCode = pollCode;
    this->listEnrg = listEnrg;
    lastFullDateTimeMask = lastDateMask = dateMask.isEmpty() ? "yyyy-MM-dd" : dateMask;
    lastFullDateTimeMask.append(" hh:mm:ss");
    this->allowDate2utc = allowDate2utc;
}


void ZbyratorDataCalculation::onAddlistOfMeters2cache(ClassManagerSharedObjects *shrdObj, const UniversalMeterSettList &activeMeters, const MyNi2model &switchedOffMeters, const bool &checkOffMeters)
{
    Q_UNUSED(switchedOffMeters);
    if(checkOffMeters){
        shrdObj->clear();
        shrdObj->clearWrite();
    }

    for(int i = 0, imax = activeMeters.size(); i < imax; i++){
        const UniversalMeterSett m = activeMeters.at(i);

        shrdObj->addMeter2meterNi2info(m.ni, m.memo, m.sn, m.coordinate, m.tariff, m.transformer);
        if(!m.sn.isEmpty())
            shrdObj->addMeter2meterSn2info(m.sn, m.memo, m.ni, m.coordinate, m.tariff, m.transformer);
    }

}
