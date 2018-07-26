#ifndef ZBYRATORDATABASEMEDIUM_H
#define ZBYRATORDATABASEMEDIUM_H

#include <QObject>
#include "src/matilda-conf/classmanagersharedobjects.h"
#include "src/zbyrator-v2/zbyratordatatypehelper.h"
#include "src/matilda/classmanagertypes.h"
#include "src/matilda/classmanagerdatabasereader.h"

class ZbyratorDatabaseMedium : public QObject
{
    Q_OBJECT
public:
    explicit ZbyratorDatabaseMedium(QObject *parent = nullptr);
    ClassManagerSharedObjects *shrdObj;

signals:
    void setPbReadEnableDisable(bool disable);

    void data2matilda(quint16 command, QVariant var);

    void uploadProgress(int val, QString txt);


    void showMess(QString);

    void operationStatus4progress(bool inProcess, QString operName);

    void appendDataDatabase(QVariantHash hash);

    void appendDataDatabaseMJ(QVariantHash hash);


    void stopAllDirect();


    void setLblWaitTxtDatabase(QString s);
    void setLblWaitTxtDatabaseMj(QString s);


public slots:
    void onThreadStarted();

    void onAlistOfMeters(quint8 meterType, UniversalMeterSettList activeMeters, MyNi2model switchedOffMeters, bool checkOffMeters);

    void data2matilda4inCMD(quint16 command, QVariant dataVar);

    void data2matildaSlot(const quint16 &command, const QVariant &dataVar);

    void stopOperationSlot();

    void data2gui(quint16 command, QVariant dataVar);

    void onCOMMAND2GUI(quint16 command, QVariantHash varHash);

    void operationStatusSlot(const bool &inProgress);

    void showMessSlot(const QString &mess);

    void sendLastCmd();

    void uploadProgressSlot(int val, QString txt);

    void setDateMask(QString dateMask);
    void setDotPos(int dotPos);

private:

    bool iNeed2cacheInfoAboutMeter(const quint16 &command, const QVariant &sendVar);
    void createDatabaseReader();

    void showOperRez(const QString &mess, const bool &isGood);

    void showOperRez(const quint16 &command, const bool &isGood);

    bool checkSqlClientReady(const quint16 &command, const QVariant &dataVar);

    bool stopAll;
    ClassManagerDatabaseReader *dbReader;

    struct OnSqlclntNotReady
    {
        quint16 lastcommand;
        QVariant lastdataVar;
        bool isValid;
        OnSqlclntNotReady() : isValid(false) {}
    } lastCmd;


    bool isSqlclientReady;
};

#endif // ZBYRATORDATABASEMEDIUM_H