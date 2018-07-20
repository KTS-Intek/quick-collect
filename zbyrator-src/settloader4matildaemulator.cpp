#include "settloader4matildaemulator.h"
#include "src/matilda/settloader4matildadefaults.h"
#include "src/matilda/settloader.h"
#include "src/matilda/settloader4matildakeys.h"

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

    //emit something changed
    //save settt
    return true;
}

QVariant SettLoader4matildaEmulator::defaultValue4zbyratorKey(const int &key)
{

    return QVariant();
}

QVariantHash SettLoader4matildaEmulator::defaultPollWdgtSett()
{
    QVariantHash h;
    h.insert("mr"   , 3 );
    h.insert("mrfa" , 2 );
    h.insert("pw"   , 3);
    h.insert("ha"   , false);
    h.insert("frwrd", false);
    h.insert("af"   , true);

    h.insert("w4e"  , false);
    h.insert("w4eRb", 11);
    h.insert("w4eRa", 555);

    h.insert("tc"   , true);
    h.insert("td"   , 90);
    return h;
}

void SettLoader4matildaEmulator::updatePollSett()
{
    const QVariantHash h = SettLoader::loadSett(SETT_ZBRTR_POLLPAGE, defaultPollWdgtSett()).toHash();

    if(h.contains("mr") &&  h.value("mr").toInt() > 0 && h.value("mr").toInt() < 30)
         hashSett.insert(SETT_POLL_METER_RETRY, h.value("mr").toInt()) ;

    if(h.contains("mrfa") && h.value("mrfa").toInt() > 0 && h.value("mrfa").toInt() < 30)
         hashSett.insert(SETT_POLL_METER_RETRY_FA, h.value("mrfa").toInt()) ;

    if(h.contains("pw") && h.value("pw").toInt() > 0 && h.value("pw").toInt() < 301)
        hashSett.insert(SETT_POLL_WAIT4POLL, h.value("pw").toInt()) ;

    if(h.contains("ha") )
        hashSett.insert(SETT_POLL_HARD_ADDRS, h.value("ha").toBool());

    if(h.contains("frwrd") )
        hashSett.insert(SETT_POLL_FRWRD_TBL_4_PLL, h.value("frwrd").toBool());

    if(h.contains("af") )
        hashSett.insert(SETT_POLL_AUTO_FRWRD_TBL, h.value("af").toBool());

    if(h.contains("w4e") )
        hashSett.insert(SETT_POLL_WAIT4EMB, h.value("w4e").toBool());

    if(h.contains("w4eRb") && h.value("w4eRb").toInt() > 0 && h.value("w4eRb").toInt() < 501)
        hashSett.insert(SETT_POLL_WAIT4EMB_RTRBFR, h.value("w4eRb").toInt());

    if(h.contains("w4eRa") && h.value("w4eRa").toInt() > 0 && h.value("w4eRa").toInt() < 501)
        hashSett.insert(SETT_POLL_WAIT4EMB_RTRAFT, h.value("w4eRa").toInt());

    if(h.contains("tc"))
        hashSett.insert(SETT_POLL_TIME_CORR_ENBL, h.value("tc").toBool() );

    if(h.contains("td") && h.value("td").toInt() > 11 && h.value("td").toInt() < 301)
        hashSett.insert(SETT_POLL_DIFF_TIME_CORR, h.value("td").toInt());

}
