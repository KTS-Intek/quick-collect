#include "zbyratorucmedium.h"
#include "src/matilda/moji_defy.h"
#include "matilda-bbb-src/matildaprotocolinitstep.h"
#include "src/shared/readwriteiodevice.cpp"

ZbyratorUcMedium::ZbyratorUcMedium(QObject *parent) : QObject(parent)
{
    MatildaProtocolInitStep::resetSpeedStat(speedStat, 23);

}

void ZbyratorUcMedium::onThreadStarted()
{
    timeObject = new IneedMoreTimeObject(this);
    connect(timeObject, SIGNAL(mWriteIneedMoreTime(quint16,qint32,qint64)), this, SLOT(mWriteIneedMoreTime(quint16,qint32,qint64)));
    connect(this, SIGNAL(restartTimeObject()), timeObject, SLOT(restart()) );

    databaseReader = new UconDatabaseReader(lastObjSett, timeObject, this);
    connect(this, &ZbyratorUcMedium::setIneedMoreTime , databaseReader, &UconDatabaseReader::setIneedMoreTime );
    connect(this, &ZbyratorUcMedium::disconnected     , databaseReader, &UconDatabaseReader::stopAllNow          );
    connect(this, &ZbyratorUcMedium::disconnected     , databaseReader, &UconDatabaseReader::stopAllSlot         );
    connect(databaseReader, &UconDatabaseReader::mWriteIneedMoreTime    , this, &ZbyratorUcMedium::mWriteIneedMoreTime);
    connect(databaseReader, &UconDatabaseReader::command2extension      , this, &ZbyratorUcMedium::command2extension  );

    databaseReader->createDatabaseClient();

    databaseReader->useThisProtocolVersion = MATILDA_PROTOCOL_VERSION;


    lastObjSett.allowCompress = true;
    lastObjSett.lastHashSumm = 1;//Md5
    lastObjSett.compressOdynRaz = false;
    this->lastObjSett.verboseMode = false;


    QTimer::singleShot(555, this, SIGNAL(sendLastCmd()) );
}

void ZbyratorUcMedium::mWriteIneedMoreTime(const quint16 &command, const qint32 &elTime, const qint64 &someData)
{
    Q_UNUSED(command);
    Q_UNUSED(elTime);
    Q_UNUSED(someData);
}

void ZbyratorUcMedium::data2matilda(quint16 command, QVariant var)
{
    speedStat.mynulo4asy = (timeObject) ? timeObject->elapsed() : 0;
    ReadWriteIODevice::readIODeviceCheckSpeed(lastObjSett, speedStat);
    decodeReadData(var, command);
}

void ZbyratorUcMedium::decodeReadData(QVariant dataVar, const quint16 &command)
{
    QVariant s_data;
    quint16 s_command = command;
    databaseReader->resetStopAll();
    switch (command) {
    case COMMAND_READ_DATABASE          :{ s_data = databaseReader->onCOMMAND_READ_DATABASE(dataVar, s_command, command, speedStat, lastObjSett); break;}

    case COMMAND_READ_DATABASE_GET_TABLES:{ s_data = databaseReader->onCOMMAND_READ_DATABASE_GET_TABLES(dataVar, s_command, command); break;}

    case COMMAND_READ_DATABASE_GET_VAL  :{ s_data = databaseReader->onCOMMAND_READ_DATABASE_GET_VAL(dataVar, s_command, command, speedStat, lastObjSett)    ; break;}

    case COMMAND_READ_METER_LOGS        :{ s_data = databaseReader->onCOMMAND_READ_METER_LOGS(dataVar, s_command, command, speedStat, lastObjSett)          ; break;}

    case COMMAND_READ_METER_LOGS_GET_TABLES:{s_data = databaseReader->onCOMMAND_READ_METER_LOGS_GET_TABLES(dataVar, s_command, command); break;}

    case COMMAND_READ_METER_LOGS_GET_VAL:{ s_data = databaseReader->onCOMMAND_READ_METER_LOGS_GET_VAL(dataVar, s_command, command, speedStat, lastObjSett)  ; break;}

    case COMMAND_READ_TABLE_HASH_SUMM:{ s_data = databaseReader->onCOMMAND_READ_TABLE_HASH_SUMM(dataVar, s_command, command)   ; break;}

    case COMMAND_WRITE_DROP_TABLE_GET_COUNT:{ s_data = databaseReader->onCOMMAND_WRITE_DROP_TABLE_GET_COUNT(dataVar, s_command, command) ; break;}

    case COMMAND_WRITE_DROP_TABLE:{ s_data = databaseReader->onCOMMAND_WRITE_DROP_TABLE(dataVar, s_command, command);         break;}

    }
    mWriteToSocket(s_data, s_command);
}

void ZbyratorUcMedium::mWriteToSocket(const QVariant &s_data, const quint16 &s_command)
{

//QIODevice *device, const QVariant &s_data, const quint16 &s_command, LastMatildaObjSett &lastObjSett, const qint64 &connSpee
//    QIODevice *f = new QIODevice();
    speedStat.lastByteWrt = ReadWriteIODevice::preparyMessage(s_data, s_command, lastObjSett.verboseMode, speedStat.connSpeed).length();//   //     qDebug() << block.toHex();
    speedStat.totalBytesTransm += speedStat.lastByteWrt;
    speedStat.writeCounter++;
    //    if(lastObjSett.verboseMode)
    if(lastObjSett.verboseMode) qDebug() << "UconMainSocket lastByteWrt= " << QTime::currentTime().toString("hh:mm:ss.zzz") <<  speedStat.lastByteWrt << s_command << speedStat.totalBytesTransm << speedStat.writeCounter;
    emit data2gui(s_command, s_data);

    emit restartTimeObject();
}
