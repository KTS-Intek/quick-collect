#include "zbyratortasksmedium.h"

#include <QDateTime>
#include <QTimer>


///[!] sharedmemory
#include "src/shared/sharedmemohelper.h"

///[!] matilda-bbb-shared
#include "matilda-bbb-src/decoders/decodematildaprotocolv5helper.h"


///[!] guisett-shared
#include "src/nongui/tableheaders.h"
#include "src/nongui/classmanagerhelper.h"


///[!] matilda-bbb-clientside
#include "src/matilda-conf/classmanagerframeddataiotypes.h"
#include "src/matilda-conf/processtasktablereadout.h"


//#include "src/matilda/moji_defy.h"
#include "definedpollcodes.h"


ZbyratorTasksMedium::ZbyratorTasksMedium(QObject *parent) : QObject(parent)
{
    sendUpdate = false;
    canSendUpdate = false;
    pageIsReady = false;
    dateMask = "yyyy-MM-dd";
    allowOnce = true;
}

ZbyratorTaskTable ZbyratorTasksMedium::getTaskTable()
{
    QByteArray readArr = SharedMemoHelper::readFromSharedMemoryArr(SharedMemoHelper::defZbyratorTaskTableMemoName(), SharedMemoHelper::defZbyratorTaskTableSemaName());

    ZbyratorTaskTable table;
    if(!readArr.isEmpty()){//no data
        QDataStream stream(&readArr, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_5_6);
        stream >> table;
    }
    return table;
}

void ZbyratorTasksMedium::onThreadStarted()
{
    canSendUpdate = true;

        onTaskTableChanged();
}

void ZbyratorTasksMedium::onTaskTableChanged()
{


    try2sendTaskTable();
    QTimer::singleShot(3333, this, SLOT(onTaskTableChanged()));

}

void ZbyratorTasksMedium::try2sendTaskTable()
{
    if(!canSendUpdate || !pageIsReady){
        sendUpdate = true;
        return;
    }
    QStringList header;
    const QStringList list = DecodeMatildaProtocolV5Helper::preparyCacheTaskTable(header, 0, 0, QList<quint8>(), QList<quint8>(), QStringList());

    QVariantHash h;
    h.insert("t", list.size());
    h.insert("s", list);
    h.insert("i", -1);
    h.insert("c", header.join("|"));

    onZbyratorTasksPageSett(h);
}

void ZbyratorTasksMedium::onPageReady()
{
    pageIsReady = true;

        onTaskTableChanged();
}

void ZbyratorTasksMedium::onZbyratorTasksPageSett(QVariantHash h)
{
    const FramedHashAnswr answr = ClassManagerHelper::readFromHashFramed(h);


    QStringList headerh;
    const MyListStringList list = ProcessTaskTableReadout::getHumanViewTasks(answr,
                                                                             answr.header,
                                                                             Tasks2human::getDefaultStatusesList(answr.header), dateMask, headerh);


//    MyListStringList list;
////   tr("NI,ID,Poll code,Priority,Created,Changed,Counter,Counter Total,Meter type,Status,Source,Ignore data,Data from,Data to,Days of the weak,Days of the month, Time from,Time to");
//// ni id pollCode prtt4pollCode dtCreated dtFinished counter counterTotal   deviceType stts rez srcId     ignorePrevData      pollDateTimeFirstIntrvl pollDateTime dow dom timtFrom timeTo


//    const QList<int> colsDt = QList<int>() << 4 << 5 << 13 << 14; //dtCreated dtFinished  //pollDateTimeFirstIntrvl pollDateTime
//    const QStringList masks = QStringList() << " hh:mm:ss.zzz" << " hh:mm:ss.zzz" << " hh:mm" << " hh:mm";

//    for(int i = 0, imax = answr.listRows.size(), jMax = masks.size(); i < imax; i++){
//        QStringList l = answr.listRows.at(i);

//        const quint8 pollCode = l.at(2).toUInt();
//        const bool convert2utc = !(pollCode == POLL_CODE_READ_END_DAY || pollCode == POLL_CODE_READ_END_MONTH || pollCode == POLL_CODE_READ_METER_LOGBOOK);

//        for(int j = 0; j < jMax; j++){
//            const int c = colsDt.at(j);
//            const QDateTime dt = QDateTime::fromMSecsSinceEpoch(l.at(c).toLongLong(0, 16));
//            if(!dt.isValid())
//                continue;

//            l.replace(c, convert2utc ? dt.toString(dateMask + masks.at(j)) : dt.toLocalTime().toString(dateMask + masks.at(j)));
//        }
//        list.append(l);
//    }


    if(!allowOnce && lastList == list)
        return;
    allowOnce = false;
    lastList = list;
    emit setZbyratorTasksPageSett(list, answr.col2data, TableHeaders::getColNamesDeviceTasks().split(","), answr.header, !answr.header.isEmpty());

}
