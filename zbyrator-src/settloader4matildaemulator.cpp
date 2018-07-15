#include "settloader4matildaemulator.h"
#include "src/matilda/settloader4matildadefaults.h"

SettLoader4matildaEmulator::SettLoader4matildaEmulator(QObject *parent) : QObject(parent)
{

}

//quint16 meterRetryMax = sLoader.loadOneSett(SETT_POLL_METER_RETRY).toUInt();
//quint16 meterRetryMaxFA = sLoader.loadOneSett(SETT_POLL_METER_RETRY_FA).toUInt();
//const bool hardAddrsn = sLoader.loadOneSett(SETT_POLL_HARD_ADDRS).toBool();
//const bool enableW4E = sLoader.loadOneSett(SETT_POLL_WAIT4EMB).toBool();
//qint32 messCountBeforeReady = sLoader.loadOneSett(SETT_POLL_WAIT4EMB_RTRBFR).toInt();
//qint32 messCountAfter = sLoader.loadOneSett(SETT_POLL_WAIT4EMB_RTRAFT).toInt();

//qint32 corDTintrvl = sLoader.loadOneSett(SETT_POLL_DIFF_TIME_CORR).toInt();


//const bool corDTallow = sLoader.loadOneSett(SETT_POLL_TIME_CORR_ENBL).toBool();


//#if defined(ENABLE_DBG_SQLTMEDIUM_TEST_MODE) || defined(ENABLE_DBG_LOCALTCP_TEST_MODE)
//const QString host = (connSett.prdvtrAddr.isEmpty() || connSett.prdvtrAddr.contains(" ") || !QUrl(connSett.prdvtrAddr).isValid()) ? "::1" : connSett.prdvtrAddr;
//const quint16 port = (connSett.prdvtrPort < 1) ? SettLoader4matildaDefaults::defServerPort() : connSett.prdvtrPort;
//#else
//const QString host = "::1";
//const quint16 port = SettLoader4matildaDefaults::defServerPort() ;
//#endif
//const qint32 timeoutB = sLoader.loadOneSett(SETT_ZIGBEE_READ_TOB).toInt();
//const qint32 timeoutG = sLoader.loadOneSett(SETT_ZIGBEE_READ_TO).toInt();

//emit setZbyratorCommunicationSett(host, port, timeoutB, timeoutG);


//const bool useForwardTable  = sLoader.loadOneSett(SETT_POLL_FRWRD_TBL_4_PLL).toBool();
//const bool avtoForwardTable = sLoader.loadOneSett(SETT_POLL_AUTO_FRWRD_TBL).toBool();
//QVariantHash forwardTableL;
//    forwardTableL = sLoader.loadOneSett(SETT_FORWARD_TABLE).toHash();

//qint32 zatrymkaDoZapuskuSek = sLoader.loadOneSett(SETT_POLL_WAIT4POLL).toInt();

QVariant SettLoader4matildaEmulator::loadOneSett(const int &key)
{
    return hashSett.value(key, SettLoader4matildaDefaults::defValue4key(key));
}

bool SettLoader4matildaEmulator::saveOneSett(const int key, const QVariant data2save)
{
    hashSett.insert(key, data2save);
    //save settt
    return true;
}
