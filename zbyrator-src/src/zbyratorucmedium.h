#ifndef ZBYRATORUCMEDIUM_H
#define ZBYRATORUCMEDIUM_H

#include <QObject>
#include "matilda-bbb-src/database/ucondatabasereader.h"

class ZbyratorUcMedium : public QObject
{
    Q_OBJECT
public:
    explicit ZbyratorUcMedium(QObject *parent = nullptr);
    IneedMoreTimeObject *timeObject;

signals:
    void setIneedMoreTime(quint16 command, qint32 elTime, qint64 someData);

    void restartTimeObject();

    void disconnected();

    void command2extension(quint16 extName, quint16 command, QVariant data);


    void data2gui(quint16 command, QVariant dataVar);

    void sendLastCmd();

public slots:
    void onThreadStarted();

    void mWriteIneedMoreTime(const quint16 &command, const qint32 &elTime, const qint64 &someData);

    void data2matilda(quint16 command, QVariant var);


    void killAllObjects();

private slots:
    void decodeReadData(QVariant dataVar, const quint16 &command);

    void mWriteToSocket(const QVariant &s_data, const quint16 &s_command);

private:
    UconDatabaseReader *databaseReader;
    LastMatildaObjSett lastObjSett;
    TcpSpeedStat speedStat;

};

#endif // ZBYRATORUCMEDIUM_H
