#include "zbyratordatabasemedium.h"
#include "zbyratordatacalculation.h"
#include "src/matilda/moji_defy.h"
#include "src/matilda/showmesshelper.h"

#include "zbyrator-src/src/zbyratorucmedium.cpp"

ZbyratorDatabaseMedium::ZbyratorDatabaseMedium(QObject *parent) : QObject(parent)
{
    isSqlclientReady = false;
    shrdObj = 0;
}

void ZbyratorDatabaseMedium::onThreadStarted()
{
    shrdObj = new ClassManagerSharedObjects(this);

    connect(this, &ZbyratorDatabaseMedium::uploadProgress, this, &ZbyratorDatabaseMedium::uploadProgressSlot);

}

void ZbyratorDatabaseMedium::onAlistOfMeters(quint8 meterType, UniversalMeterSettList activeMeters, MyNi2model switchedOffMeters, bool checkOffMeters)
{
    Q_UNUSED(meterType);
    if(shrdObj)
        ZbyratorDataCalculation().onAddlistOfMeters2cache(shrdObj, activeMeters, switchedOffMeters, checkOffMeters);
}

void ZbyratorDatabaseMedium::data2matilda4inCMD(quint16 command, QVariant dataVar)
{
    operationStatusSlot(true);

    shrdObj->clearWrite();

    stopAll = false;
    switch(command){

    case COMMAND_READ_DATABASE:{
        QVariantHash hash = dataVar.toHash();
        hash.insert("gcl", true);
        dataVar = hash;

        hash.remove("gcl");
        shrdObj->lastPollCode = hash.value("code").toUInt();
        shrdObj->hashMemoWrite.insert(COMMAND_READ_DATABASE, hash);

        iNeed2cacheInfoAboutMeter(command, dataVar);
        break; }



    case COMMAND_READ_DATABASE_GET_TABLES:{
        QVariantHash hash = dataVar.toHash();
        shrdObj->hashMemoWrite.insert(COMMAND_READ_DATABASE_GET_VAL, hash);
        hash.remove("enrg");
        hash.remove("tarif");
        shrdObj->hashMemoWrite.insert(COMMAND_READ_DATABASE_GET_TABLES, hash);

        iNeed2cacheInfoAboutMeter(command, dataVar);

        break;}

    case COMMAND_READ_METER_LOGS:{
        QVariantHash hash = dataVar.toHash();
        hash.insert("gcl", true);
        dataVar = hash;

        hash.remove("gcl");
        shrdObj->hashMemoWrite.insert(COMMAND_READ_METER_LOGS, hash);

        if(iNeed2cacheInfoAboutMeter(command, dataVar))
            return;
        break;}

    case COMMAND_READ_METER_LOGS_GET_TABLES:{
        shrdObj->hashMemoWrite.insert(COMMAND_READ_METER_LOGS_GET_TABLES, dataVar.toHash());
        shrdObj->hashMemoWrite.insert(COMMAND_READ_METER_LOGS_GET_VAL, dataVar.toHash());

        iNeed2cacheInfoAboutMeter(command, dataVar);


        break;}

    case COMMAND_WRITE_DROP_TABLE_GET_COUNT:{
        QVariantHash hash = dataVar.toHash();
        if((shrdObj->timeOutMsec / 2) > 3000)
            hash.insert("msec", shrdObj->timeOutMsec / 2);
        else
            hash.insert("msec", shrdObj->timeOutMsec);

        shrdObj->hashMemoWrite.insert(COMMAND_WRITE_DROP_TABLE_GET_COUNT, hash);
        dataVar = hash;
        break;}


    case COMMAND_READ_DATABASE_PREPAID:{
        QVariantHash hash = dataVar.toHash();

        hash.remove("gcl");
        shrdObj->hashMemoWrite.insert(COMMAND_READ_DATABASE_PREPAID, hash);

        iNeed2cacheInfoAboutMeter(command, dataVar);


        break; }
    }

    if(checkSqlClientReady(command, dataVar))//тут починатиметься зчитування
        data2matildaSlot(command, dataVar);
}

void ZbyratorDatabaseMedium::data2matildaSlot(const quint16 &command, const QVariant &dataVar)
{
    if(stopAll){
        operationStatusSlot(false);
        showMessSlot(ShowMessHelper::messFromCode(MESS_OPERATION_ABORTED));
        return;
    }
    lastCmd.lastcommand = command;
    emit data2matilda(command, dataVar);//signal to database


}

void ZbyratorDatabaseMedium::stopOperationSlot()
{
    stopAll = true;
    emit stopAllDirect();
}

void ZbyratorDatabaseMedium::data2gui(quint16 command, QVariant dataVar)
{
    bool rezIsGood = true;
    int messCode = MESS_NO_MESS;
    operationStatusSlot(true);

     switch (command) {

     case COMMAND_READ_DATABASE              :{ messCode = dbReader->onCOMMAND_READ_DATABASE(dataVar.toHash(), rezIsGood)                      ; break;}

     case COMMAND_READ_DATABASE_GET_TABLES   :{ messCode = dbReader->onCOMMAND_READ_DATABASE_GET_TABLES(dataVar.toHash(), rezIsGood)           ; break;}

     case COMMAND_READ_DATABASE_GET_VAL      :{ messCode = dbReader->onCOMMAND_READ_DATABASE_GET_VAL(dataVar.toHash(), rezIsGood)              ; break;}

     case COMMAND_READ_METER_LOGS            :{ messCode = dbReader->onCOMMAND_READ_METER_LOGS(dataVar.toHash(), rezIsGood)                    ; break;}

     case COMMAND_READ_METER_LOGS_GET_TABLES :{ messCode = dbReader->onCOMMAND_READ_METER_LOGS_GET_TABLES(dataVar.toHash(), rezIsGood)         ; break;}

     case COMMAND_READ_METER_LOGS_GET_VAL    :{ messCode = dbReader->onCOMMAND_READ_METER_LOGS_GET_VAL(dataVar.toHash(), rezIsGood)            ; break;}
     default:{
         qDebug() << "data2gui unknown command " << command << dataVar;
         showMessSlot(ShowMessHelper::addWithFontColorRed(tr("Unknown command: 0x%1").arg(QString::number(command, 16).toUpper())));
         return;}
     }


     if(rezIsGood && messCode == MESS_OPERATION_IN_PROGRESS)
         return;


     if(!(rezIsGood && (command == COMMAND_ERROR_CODE || command == COMMAND_ERROR_CODE_EXT))){

         if(messCode != MESS_NO_MESS && messCode != MESS_OPERATION_IN_PROGRESS)
             showMessSlot(ShowMessHelper::messFromCode(messCode));
         showOperRez(command, rezIsGood);
     }
     if(!(command == COMMAND_AUTHORIZE && rezIsGood)){
         operationStatusSlot(false);
     }

     if(!stopAll){
         if(command != COMMAND_ERROR_CODE_EXT)
             emit operationStatus4progress(false, ShowMessHelper::name4command(command));
     }

}

void ZbyratorDatabaseMedium::onCOMMAND2GUI(quint16 command, QVariantHash varHash)
{    
    if(command == COMMAND_READ_METER_LOGS)
        emit appendDataDatabaseMJ(varHash);
    else
        emit appendDataDatabase(varHash);
}

void ZbyratorDatabaseMedium::operationStatusSlot(const bool &inProgress)
{
    emit setPbReadEnableDisable(inProgress);

}

void ZbyratorDatabaseMedium::showMessSlot(const QString &mess)
{
    if(!mess.isEmpty())
        emit showMess(mess);
}

void ZbyratorDatabaseMedium::sendLastCmd()
{
    if(lastCmd.isValid)
        data2matildaSlot(lastCmd.lastcommand, lastCmd.lastdataVar);
    lastCmd.lastdataVar = false;
    lastCmd.isValid = false;
}

void ZbyratorDatabaseMedium::uploadProgressSlot(int val, QString txt)
{
    const QString s = QString("%1, %2 %").arg(txt).arg(val);
    if(lastCmd.lastcommand == COMMAND_READ_METER_LOGS)
        emit setLblWaitTxtDatabaseMj(s);
    else
        emit setLblWaitTxtDatabase(s);

}

void ZbyratorDatabaseMedium::setDateMask(QString dateMask)
{
    if(shrdObj)
        shrdObj->dateMask = dateMask;

}

void ZbyratorDatabaseMedium::setDotPos(int dotPos)
{
    if(shrdObj)
        shrdObj->komaPos = dotPos;
}

bool ZbyratorDatabaseMedium::iNeed2cacheInfoAboutMeter(const quint16 &command, const QVariant &sendVar)
{
    Q_UNUSED(command);
    Q_UNUSED(sendVar);
    shrdObj->commandSequenceAfter = 0;
    shrdObj->dataSequenceAfter.clear();
    return false;
}

void ZbyratorDatabaseMedium::createDatabaseReader()
{
    dbReader = new ClassManagerDatabaseReader(shrdObj, this);

     connect(dbReader, &ClassManagerDatabaseReader::data2matildaSlot, this, &ZbyratorDatabaseMedium::data2matildaSlot);
     connect(dbReader, SIGNAL(onCOMMAND2GUI(quint16,QVariantHash)), this, SLOT(onCOMMAND2GUI(quint16,QVariantHash)));
     connect(dbReader, &ClassManagerDatabaseReader::uploadProgress, this, &ZbyratorDatabaseMedium::uploadProgress);

}

void ZbyratorDatabaseMedium::showOperRez(const QString &mess, const bool &isGood)
{
    //emit enablePbOperations()
    if(mess.isEmpty()){
        qDebug() << "ClassManager::showOperRez " << mess << isGood;
        return;
    }
    showMessSlot(ShowMessHelper::addRez2endOfLine(mess, isGood));
}

void ZbyratorDatabaseMedium::showOperRez(const quint16 &command, const bool &isGood)
{
    if(ShowMessHelper::name4command(command).isEmpty()){
        qDebug() << "ClassManager::showOperRez( name is empty" << command << isGood;
    }
    showOperRez(ShowMessHelper::name4command(command), isGood);
}

bool ZbyratorDatabaseMedium::checkSqlClientReady(const quint16 &command, const QVariant &dataVar)
{
    if(isSqlclientReady)
        return true;

    isSqlclientReady = true;

    lastCmd.lastcommand = command;
    lastCmd.lastdataVar = dataVar;
    lastCmd.isValid = true;

    ZbyratorUcMedium *m = new ZbyratorUcMedium;
    QThread *t = new QThread;

    m->moveToThread(t);

    connect(this, SIGNAL(destroyed(QObject*)), m, SLOT(deleteLater()) );
    connect(m, SIGNAL(destroyed(QObject*)), t, SLOT(quit()) );
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()) );
    connect(t, SIGNAL(started()), m, SLOT(onThreadStarted()) );

    connect(this, SIGNAL(stopAllDirect()), m, SIGNAL(disconnected()));
    connect(this, SIGNAL(data2matilda(quint16,QVariant)), m, SLOT(data2matilda(quint16,QVariant)) );
    connect(m, SIGNAL(data2gui(quint16,QVariant)), this, SLOT(data2gui(quint16,QVariant)) );
    connect(m, SIGNAL(sendLastCmd()), this, SLOT(sendLastCmd()) );
    t->start();

    createDatabaseReader();
    return false;
}
