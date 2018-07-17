#ifndef IFACESETTLOADER_H
#define IFACESETTLOADER_H

#include <QObject>
#include <QVariantHash>
#include <QStringList>

class IfaceSettLoader : public QObject
{
    Q_OBJECT
public:
    explicit IfaceSettLoader(QObject *parent = nullptr);

    static QVariantHash getM2mProfiles();

    static void saveM2mProfiles(const QVariantHash &h);

    static QStringList getProfileNames(QStringList &icoList);

    static QVariantHash getProfileSett(const QString &profileName);

    static void saveOnlyThisProfiles(const QStringList &listProfileNames);

    QString getLastConnDevInfo(const bool &lastIsDirrConn) const;

    QVariantHash getDev2conn();

signals:
    void onThisObjName(QString s);

    void onThisIp(QString s);

    void onThisPort(int i);

    void onThisMac(QString s);

    void onThisMode(int i);

    void onThisObjName_2(QString s);

    void onTheseM2mParams(QString ip, int port);

    void onTheseLoginPasswd(QString login, QString passwd);

    void onThisUseMacAddr2conn(bool useMacAdd2conn);

    void onThisTimeout(int sec);

    void onThisM2mTimeout(int sec);

public slots:
    void setThisObjName(QString s);

    void setThisIp(QString s);

    void setThisPort(int i);

    void setThisMac(QString s);

    void setThisMode(int i);

    void setThisObjName_2(QString s);

    void setTheseM2mParams(QString ip, int port);

    void setTheseLoginPasswd(QString login, QString passwd);

    void setThisUseMacAddr2conn(bool useMacAdd2conn);

    void setThisTimeout(int sec);

    void setThisM2mTimeout(int sec);


    void setConfPageSett(QString objN, QString login, QString passwd, QString add, quint16 port, int timeOut, bool add2list, bool useMac, QString macAddr, bool useMacAddr2conn);

    void addThisDev2connPage(QVariantHash connSett);

private:
    struct LastIfaceSett
    {
        QString objName;
        QString ip;
        int port;
        QString mac;
        int mode;
        QString objName2;

        QString login;
        QString passwd;

        bool useMacAddr2conn;

        QString m2mip;
        int m2mport;

        int timeoutSec;
        int m2mtimeoutSec;

        LastIfaceSett() {}
    } myIface;
};

#endif // IFACESETTLOADER_H
