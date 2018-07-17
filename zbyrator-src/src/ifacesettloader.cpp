#include "ifacesettloader.h"
#include "src/matilda/settloader.h"
#include <QDataStream>

IfaceSettLoader::IfaceSettLoader(QObject *parent) : QObject(parent)
{

}

QVariantHash IfaceSettLoader::getM2mProfiles()
{
    QByteArray arr = SettLoader::loadSett(SETT_ZBRTR_MTD_PRFLS).toByteArray();
    QVariantHash h;

    if(!arr.isEmpty()){
        for(int i = 0; i < 11; i++)
            arr = qUncompress(arr);

        QDataStream stream(&arr, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_5_6);
        stream >> h;

    }
    return h;
}

void IfaceSettLoader::saveM2mProfiles(const QVariantHash &h)
{
    QByteArray arr;
    if(true){
        QDataStream stream(&arr, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_5_6);
        stream << h;
    }
    for(int i = 0; i < 11; i++)
        arr = qCompress(arr);

    SettLoader::saveSett(SETT_ZBRTR_MTD_PRFLS, arr);
}

QStringList IfaceSettLoader::getProfileNames(QStringList &icoList)
{

    const QVariantHash h = getM2mProfiles();
    QList<QString> lk = h.keys();
    std::sort(lk.begin(), lk.end());

    QStringList l;
    icoList.clear();
    //MAC && Direct Matilda <objN>\t<login>
    for(int i = 0, iMax = lk.size(); i < iMax; i++){
        const QVariantHash oh = h.value(lk.at(i)).toHash();
        if(oh.isEmpty())
            continue;

        QString back;
        if(!oh.value("useMac", true).toBool()){
            back.append("\t");
            icoList.append(":/katynko/svg/network-disconnect.svg");
        }else{
            icoList.append(":/katynko/svg2/lc_browseview.svg");
        }
        l.append(QString("%1\t%2%3").arg(oh.value("objN").toString()).arg(oh.value("l").toString()).arg(back));
//        else
//            l.append(QString("%1\t%2\t%3").arg(oh.value("objN").toString()).arg(oh.value("l").toString()).arg(ShowMessHelper::prettyIpMatildaView(oh.value("a").toString(), oh.value("p").toInt())));

    }
    l.removeDuplicates();
    return l;
}

QVariantHash IfaceSettLoader::getProfileSett(const QString &profileName)
{
    const QStringList l = profileName.split("\t");
    return (l.isEmpty()) ? QVariantHash() : getM2mProfiles().value(l.first()).toHash();
}

void IfaceSettLoader::saveOnlyThisProfiles(const QStringList &listProfileNames)
{

    const QVariantHash h = getM2mProfiles();

    QVariantHash hh;

    for(int i = 0, iMax = listProfileNames.size(); i < iMax; i++){
        QString key = listProfileNames.at(i);
        key = key.split("\t").first();
        const QVariantHash oh = h.value(key).toHash();
        if(oh.isEmpty())
            continue;

        hh.insert(key, oh);
    }

    saveM2mProfiles(hh);


}

QString IfaceSettLoader::getLastConnDevInfo(const bool &lastIsDirrConn) const
{

    return lastIsDirrConn ? QString("[%1]:%2, %3, %4")
                            .arg(myIface.ip.simplified().trimmed())
                            .arg(myIface.port)
                            .arg(myIface.login)
                            .arg(myIface.objName) :

                            QString("[%1]:%2, %3, %4, %5")
                            .arg(myIface.m2mip.simplified().trimmed())
                            .arg(myIface.m2mport)
                            .arg(myIface.login)
                            .arg(myIface.objName2)
                            .arg(myIface.mac);
}

QVariantHash IfaceSettLoader::getDev2conn()
{
    QVariantHash h;
    h.insert("pas"  , myIface.passwd);
    h.insert("l"    , myIface.login);

    if(myIface.mode == 1){
        h.insert("useMac", true);//use M2M service
        h.insert("objN" , myIface.objName2);
        h.insert("a"    , myIface.m2mip);
        h.insert("p"    , myIface.m2mport);
        h.insert("mac"  , myIface.mac);
        h.insert("t"    , myIface.m2mtimeoutSec);
        h.insert("cmMAC", myIface.useMacAddr2conn);// ui->rbUseMac->isChecked());


    }else{
        h.insert("objN" , myIface.objName);
        h.insert("a"    , myIface.ip);
        h.insert("p"    , myIface.port);
        h.insert("t"    , myIface.timeoutSec);
    }
    return h;
}

void IfaceSettLoader::setThisObjName(QString s)
{
    myIface.objName = s;
    emit onThisObjName(s);
}

void IfaceSettLoader::setThisIp(QString s)
{
    myIface.ip = s;
    emit onThisIp(s);
}

void IfaceSettLoader::setThisPort(int i)
{
    myIface.port = i;
    emit onThisPort(i);
}

void IfaceSettLoader::setThisMac(QString s)
{
    myIface.mac = s;
    emit onThisMac(s);
}

void IfaceSettLoader::setThisMode(int i)
{
    myIface.mode = i;
    emit onThisMode(i);
}

void IfaceSettLoader::setThisObjName_2(QString s)
{
    myIface.objName2 = s;
    emit onThisObjName_2(s);
}

void IfaceSettLoader::setTheseM2mParams(QString ip, int port)
{
    myIface.m2mip = ip;
    myIface.m2mport = port;
    emit onTheseM2mParams(ip, port);
}

void IfaceSettLoader::setTheseLoginPasswd(QString login, QString passwd)
{
    myIface.login = login;
    myIface.passwd = passwd;

    emit onTheseLoginPasswd(login, passwd);
}

void IfaceSettLoader::setThisUseMacAddr2conn(bool useMacAdd2conn)
{
    myIface.useMacAddr2conn = useMacAdd2conn;
    emit onThisUseMacAddr2conn(useMacAdd2conn);
}

void IfaceSettLoader::setThisTimeout(int sec)
{
    myIface.timeoutSec = sec;
    emit onThisTimeout(sec);
}

void IfaceSettLoader::setThisM2mTimeout(int sec)
{
    myIface.m2mtimeoutSec = sec;
    emit onThisM2mTimeout(sec);
}

void IfaceSettLoader::setConfPageSett(QString objN, QString login, QString passwd, QString add, quint16 port, int timeOut, bool add2list, bool useMac, QString macAddr, bool useMacAddr2conn)
{
    Q_UNUSED(add2list);

    setThisMode(useMac ? 1 : 0);
    if(useMac){
        setThisObjName_2(objN);
        setThisMac(macAddr);
        setThisUseMacAddr2conn(useMacAddr2conn);
        setTheseM2mParams(add, port);
        setThisM2mTimeout(timeOut);
    }else{
        setThisObjName(objN);
        setThisIp(add);
        setThisPort(port);
        setThisTimeout(timeOut);
    }
    setTheseLoginPasswd(login, passwd);

}

void IfaceSettLoader::addThisDev2connPage(QVariantHash connSett)
{

    setTheseLoginPasswd(connSett.value("l").toString(), connSett.value("pas").toString());
    setThisObjName(connSett.value("objN").toString());
    setThisObjName_2(connSett.value("objN").toString());

    setThisMode(connSett.value("useMac").toBool() ? 1 : 0);

    if(connSett.value("useMac").toBool()){
        setTheseM2mParams(connSett.value("a").toString(), connSett.value("p").toInt());
        setThisMac(connSett.value("mac").toString());

        setThisUseMacAddr2conn(connSett.value("cmMAC").toBool());
        setThisM2mTimeout( connSett.value("t").toInt());

        setThisTimeout(15);
        setThisIp("");
        setThisPort(9090);

    }else{
        setThisIp(connSett.value("a").toString());
        setThisPort(connSett.value("p").toInt());
        setThisTimeout(connSett.value("t").toInt());


        setTheseM2mParams("", 65000);
        setThisMac("");
        setThisM2mTimeout(15);
        setThisUseMacAddr2conn(false);

    }

}
