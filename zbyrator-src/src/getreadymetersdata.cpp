#include "getreadymetersdata.h"
//#include "src/matilda/moji_defy.h"
#include "src/meter/definedpollcodes.h"
#include "src/zbyrator-v2/quickpollhelper.h"
#include "src/meter/zbyratorhelper.h"
#include "src/matilda/matildalimits.h"


GetReadyMetersData::GetReadyMetersData(QObject *parent) : QObject(parent)
{
    stopAll = false;
    dbProcessingHelper = 0;
}

quint8 GetReadyMetersData::meterTypeFromPollCode(const quint8 &code)
{
    quint8 meter = UC_METER_UNKNOWN;
    switch(code){
    case POLL_CODE_READ_END_MONTH   :
    case POLL_CODE_READ_END_DAY     :
    case POLL_CODE_READ_METER_LOGBOOK     :
    case POLL_CODE_READ_METER_STATE :
    case POLL_CODE_READ_POWER       :
    case POLL_CODE_READ_VOLTAGE     :
    case POLL_CODE_READ_TOTAL       : meter = UC_METER_ELECTRICITY;


    }

    return meter;
}

void GetReadyMetersData::setPollSett(QDateTime dtFrom, QDateTime dtTo, quint8 pollCode, quint8 meterType)
{
    lPollSett.dtFrom = dtFrom;
    lPollSett.dtTo = dtTo.isValid() ? dtTo : QDateTime::currentDateTime();
    lPollSett.pollCode = pollCode;
    lPollSett.meterType = meterType;
}

void GetReadyMetersData::setMetersList(UniversalMeterSettList list)
{
    meters.clear();
    for(int i = 0, imax = list.size(); i < imax; i++){
        if(list.at(i).meterType == lPollSett.meterType)
            meters.append(list.at(i));
    }
}

void GetReadyMetersData::onThreadStarted()
{
    if(!stopAll){

        if( meters.isEmpty()){
            QTimer::singleShot(111, this, SLOT(onThreadStarted()) );
            return;
        }

        addAllMeters2table();


        OneProfileSett oneprofile;

        switch(lPollSett.meterType){
        case UC_METER_ELECTRICITY: oneprofile = DefParams4zbyrator::defVal4prttProfilesElectricity().profileSett.value(lPollSett.pollCode); break;
        case UC_METER_WATER: oneprofile = DefParams4zbyrator::defVal4prttProfilesWater().profileSett4WtrMeter.value("default").profileSett; break;
        }


        oneprofile.prtt = 0;
        oneprofile.onTrue = true;
        oneprofile.glbn = QuickPollHelper::getDepth4pollCode(lPollSett.pollCode, lPollSett.dtFrom, lPollSett.dtTo);


        doCalculation(oneprofile);
    }
    deleteLater();
}

void GetReadyMetersData::stopAllDirect()
{
    if(stopAll)
        return;
    stopAll = true;
    emit closeDbConnection();
}

void GetReadyMetersData::doCalculation(const OneProfileSett &oneprofile)
{
    bool dbChecked = false;
    const quint8 pollCode = lPollSett.pollCode;
    const QDateTime currDateTime = lPollSett.dtTo;


    quint32 intrvl = oneprofile.intrvl; //= schedule4electricityMeters.hashProfiles.value(pollCode).intrvl;
    qint64 timeLeft;
    const QDateTime dateTimeFrom = MeterSchedulerHelper::getDateTimeFrom4kftnt(currDateTime, oneprofile.kftnt, intrvl, timeLeft);
    if(!dateTimeFrom.isValid()){
        emit appendMess(tr("Bad date("));
        return;
    }

    const QDateTime dtPoll = MeterSchedulerHelper::getDateTimePoll(dateTimeFrom, oneprofile.kftnt, intrvl, timeLeft);
    if(!dtPoll.isValid()){
        emit appendMess(tr("Bad date("));
        return;
    }


    bool createTables = true;
    bool meterListChanged = true;//if true: search for ni without history

    const qint32 glybokoIntrvl = ZbyratorHelper::getCorrectValue(oneprofile.glbn, MAX_GLYBYNA);

    const PollDateMemoExt pollDtMemo = ZbyratorHelper::getDateTimeMemo4poll(dtPoll, pollCode, oneprofile.kftnt, intrvl,  glybokoIntrvl, false, lPollSett.dtFrom);

    if(!pollDtMemo.pollDateTime.isValid() || !pollDtMemo.pollDateTimeFirstIntrvl.isValid() || !pollDtMemo.pollDateTimeNextIntrvl.isValid()){

        return;
    }

    if(!dbChecked)
        dbChecked = checkDbIsOpen();

    //NIs that are ready for poll,
    QStringList listNi4poll = MeterSchedulerHelper::getNIlist4poll(meters, HashNi2MapHistory(), pollCode, false, pollDtMemo.pollDateTime) ;

    if(stopAll)
        return;
    if(meterListChanged){

        if(!listNi4poll.isEmpty()){

            if(createTables)
                createTables = !dbProcessingHelper->createTableFromOld2New(pollCode, pollDtMemo.pollDateTime, pollDtMemo.depth, currDateTime, pollDtMemo.tableName);

            if(!stopAll){
//                const QStringList memo = listNi4poll;

                const MissingData md = dbProcessingHelper->getMissingData(MyUcmMeters::selectTheseNi(meters, listNi4poll), pollCode, pollDtMemo.fullTableName, oneprofile.glbn, -1, true);
                listNi4poll = md.niWithMissingData;


            }

        }
    }else{// значить нове опитування, даних точно не повинно бути в БД
     //необхідно відкинути лічильники що не підтримують вибраний код опитування
        //I must create a dbProcessingHelper object first
//        const QStringList memo = listNi4poll;

        if(stopAll)
            return;
        const MissingData md = MeterSchedulerHelper::getMetersWithSupportedPollCode(dbProcessingHelper, MyUcmMeters::selectTheseNi(meters, listNi4poll), pollCode);
       listNi4poll = md.niWithMissingData;


    }

    if(!stopAll)
        emit onProcessingEnds(listNi4poll);
    if(!listNi4poll.isEmpty()){
        if(stopAll)
            return;

        //можливо краще почати створювати таблиці тут, що опитування тоді швидше почалось
        if(createTables)
            dbProcessingHelper->createTableFromOld2New(pollCode, pollDtMemo.pollDateTime, pollDtMemo.depth, currDateTime, pollDtMemo.tableName);


    }

    return;
}

bool GetReadyMetersData::checkDbIsOpen()
{
    if(!dbProcessingHelper){

        dbProcessingHelper = new DataProcessingHelper(false, this);

        connect(this, SIGNAL(closeDbConnection()), dbProcessingHelper, SIGNAL(closeDbConnection()) );
//        connect(this, SIGNAL(allowDatabase()), dbProcessingHelper, SLOT(allowDatabase()) );
        connect(dbProcessingHelper, SIGNAL(databaseEvent(QString)), this, SIGNAL(appendMess(QString)) );
    }
    return dbProcessingHelper->checkDbIsOpen();
}

void GetReadyMetersData::addAllMeters2table()
{
    const quint8 meterType = meterTypeFromPollCode(lPollSett.pollCode);
    if(true){
        UniversalMeterSettList list;

        for(int i = 0, imax = meters.size(); i < imax; i++){
            if(meters.at(i).meterType == meterType && meters.at(i).pollEnbl)
                list.append(meters.at(i));
        }

        meters = list;
    }
    //tr("Meter;NI;Memo;Poll;Has data").split(";"));

    MyListStringList list;
    for(int i = 0, imax = meters.size(); i < imax; i++){
        const UniversalMeterSett m = meters.at(i);
        QStringList l;
        l.append(m.version.isEmpty() ? m.model : QString("%1, %2").arg(m.model).arg(m.version));
//        l.append(m.sn);
        l.append(m.ni);
        l.append(m.memo);
        l.append("-");//poll no
        l.append("?");//has data ? - unknown, + - has all data, ! - not all

        list.append(l);
    }
    const QStringList header = tr("Meter;NI;Memo;Poll;Has data").split(";");
    emit allMeters2selectWdgt(list, QVariantMap(), header, header, true);

}

