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


//QList<int> ZbyratorDataCalculation::getSpecialPollCodes()
//{
//    return QList<int>(); << POLL_CODE_READ_METER_STATE << POLL_CODE_WTR_METER_STATE;

//}

//void ZbyratorDataCalculation::processSpecialPollCode(const int &code, int &insertSnNiCorrection, QHash<QString, QString> &strhash)
//{
//    if(code == POLL_CODE_WTR_METER_STATE){


//        const QStringList vls = strhash.value("vls").split(" ", QString::SkipEmptyParts);
//        if(vls.size() > 3){
//            strhash.insert("meter_bat", vls.at(0));
//            strhash.insert("modem_bat", vls.at(1));
//            strhash.insert("meter_bat_v", vls.at(2));
//            strhash.insert("modem_bat_v", vls.at(3));

//            const QVariantHash sheduler = MeterStateHelper::waterPrmsLine2hash(strhash.value("prm"));


//            const QString profilelinejson = QString(QJsonDocument(QJsonObject::fromVariantHash(sheduler)).toJson(QJsonDocument::Compact));
//            strhash.insert("profile_params", profilelinejson);

//        }


//    }else{
//         if(code == POLL_CODE_READ_METER_STATE){
//             const QStringList vls = strhash.value("vls").split(" ");
//             if(vls.size() > 4){///home/hello_zb/My_Prog/Qt5_progs/TESTS/Matilda-units/gui/guisett-shared-core/src/nongui/classmanagerhelper.h
//                 QVariantList listOneMeter;
////                 ClassManagerHelper::addVls2listPretty(listOneMeter, strhash.value("vls"), shrdObj->dateMask);

//                 const int fromindx = MeterStateHelper::getMeterStateHeaderKeys(false).indexOf("balance");
//                 const QStringList lk = MeterStateHelper::getMeterStateHeaderKeys(false).mid(fromindx);//                             QString("balance last_charge balance_charge balance_counter tarif_values");

//                 for(int j = 0, jmax = lk.size(), jmax2 = listOneMeter.size(); j < jmax && j < jmax2; j++)
//                     strhash.insert(lk.at(j), listOneMeter.at(j).toString());

//             }
//             strhash.insert("meter_model", strhash.take("model"));
//             strhash.insert("params", strhash.value("prm"));
//             insertSnNiCorrection = 1;

//         }
//    }

//}

//-----------------------------------------------------------------------

QStringList ZbyratorDataCalculation::getHeader4specialPollCode(const int &code)
{
    QStringList columnList;
    columnList = (code == POLL_CODE_READ_METER_STATE) ? MeterStateHelper::getMeterStateHeaderKeys(false) : MeterStateHelper::getWaterMeterStateHeaderKeys(false);
//            columnList.removeFirst();
    columnList.replace(0, "msec");
    return columnList;
}

//-----------------------------------------------------------------------

QString ZbyratorDataCalculation::gimmeSn4thisNI(const QString &ni)
{
    return gimmeSn4thisNIbyDevType(ni, myPollParams.deviceType);
}

//-----------------------------------------------------------------------

QString ZbyratorDataCalculation::gimmeSn4thisNIbyDevType(const QString &ni, const quint8 &deviceType)
{
    const QString snhash = deviceType2ni2sn.value(deviceType).value(ni);

    return snhash;
}


//-----------------------------------------------------------------------

void ZbyratorDataCalculation::onMeterPollCancelled(QString ni, QString stts, qint64 msec)
{
//    ClassManagerSharedObjects *shrdObj = shrdObjElectricity;
//    const QString sn = shrdObjElectricity->electricityMeter.hashMeterNi2info.value(ni).sn ;
    const QString sn = gimmeSn4thisNI(ni);// shrdObj->electricityMeter.hashMeterNi2info.value(ni).sn ;//water?

    MyListHashString listHash;

    const QDateTime dt = QDateTime::fromMSecsSinceEpoch(msec);

    qint64 msecMinus = 0;
    switch(myPollParams.select.pollCode){
    case POLL_CODE_WTR_END_DAY      :
    case POLL_CODE_WTR_END_MONTH    :

    case POLL_CODE_READ_END_DAY     :
    case POLL_CODE_READ_END_MONTH   : msecMinus = (qint64)dt.toLocalTime().offsetFromUtc() * 1000;
    }
    QHash<QString,QString> h;
    h.insert("msec", QString::number(dt.toUTC().toMSecsSinceEpoch() - msecMinus));
    h.insert("stts", stts);
//    h.insert("meter_sn", sn);

    listHash.append(h);

    appendMeterData(ni, sn, listHash);
}

//-----------------------------------------------------------------------

void ZbyratorDataCalculation::appendMeterData(QString ni, QString sn, MyListHashString data)
{
    addKeyValue2list(data, "meter_sn", sn);

    appendPollDeviceData(ni, data);


}

//-----------------------------------------------------------------------

void ZbyratorDataCalculation::onUCEMeterSettingsChanged(UCEMeterSettings settings)
{
    QList<UCPollDeviceSettings> polldevl;
    for(int i = 0, imax = settings.eMeterContainer.size(); i < imax; i++){
        polldevl.append(settings.eMeterContainer.at(i).baseSettings);
    }
    setThesePollDevs(polldevl, UC_METER_ELECTRICITY);
}

//-----------------------------------------------------------------------

void ZbyratorDataCalculation::onUCWMeterSettingsChanged(UCWMeterSettings settings)
{
    QList<UCPollDeviceSettings> polldevl;
    for(int i = 0, imax = settings.wMeterContainer.size(); i < imax; i++){
        polldevl.append(settings.wMeterContainer.at(i).baseSettings);
    }
    setThesePollDevs(polldevl, UC_METER_WATER);
}

void ZbyratorDataCalculation::setThesePollDevs(const QList<UCPollDeviceSettings> &pollDevl, const quint8 &deviceType)
{
    if(deviceType == UC_METER_UNKNOWN)
        return;

    removeTheseDeviceType(deviceType);

    QHash<QString,QString> devices;
    QHash<QString,QString> alldevices = deviceType2ni2sn.value(UC_METER_UNKNOWN);

    for(int i = 0, imax = pollDevl.size(); i < imax; i++){
        const QString ni = pollDevl.at(i).ni;
        const QString sn = pollDevl.at(i).sn;

        devices.insert(ni, sn);
        alldevices.insert(ni, sn);
    }

    deviceType2ni2sn.insert(deviceType, devices);

}

//-----------------------------------------------------------------------

void ZbyratorDataCalculation::removeTheseDeviceType(const quint8 &deviceType)
{
    if(deviceType == UC_METER_UNKNOWN)
        return;
    const QList<QString> lk = deviceType2ni2sn.value(deviceType).keys();

    if(lk.isEmpty())
        return;

    deviceType2ni2sn.remove(deviceType);
    QHash<QString,QString> alldevices = deviceType2ni2sn.value(UC_METER_UNKNOWN);

    for(int i = 0, imax = lk.size(); i < imax; i++){
        alldevices.remove(lk.at(i));
    }
    deviceType2ni2sn.insert(UC_METER_UNKNOWN, alldevices);

}

//-----------------------------------------------------------------------




