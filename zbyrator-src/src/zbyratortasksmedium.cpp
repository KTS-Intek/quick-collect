#include "zbyratortasksmedium.h"
#include "src/shared/sharedmemohelper.h"
#include "matilda-bbb-src/decodematildaprotocolv5helper.h"
#include "src/matilda/classmanagerhelper.cpp"
//#include "src/matilda/moji_defy.h"
#include "src/meter/definedpollcodes.h"
#include <QDateTime>

#include "src/widgets/tableheaders.h"

ZbyratorTasksMedium::ZbyratorTasksMedium(QObject *parent) : QObject(parent)
{
    sendUpdate = false;
    canSendUpdate = false;
    pageIsReady = false;
    dateMask = "yyyy-MM-dd";
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
    if(!canSendUpdate || !pageIsReady){
        sendUpdate = true;
        return;
    }
    QStringList header;
    const QStringList list = DecodeMatildaProtocolV5Helper::preparyCache(header);

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

    MyListStringList list;
//   tr("NI,ID,Poll code,Priority,Created,Changed,Counter,Counter Total,Meter type,Status,Source,Ignore data,Data from,Data to,Days of the weak,Days of the month, Time from,Time to");
// ni id pollCode prtt4pollCode dtCreated dtFinished counter counterTotal   meterType stts rez srcId     ignorePrevData      pollDateTimeFirstIntrvl pollDateTime dow dom timtFrom timeTo


    const QList<int> colsDt = QList<int>() << 4 << 5 << 13 << 14; //dtCreated dtFinished  //pollDateTimeFirstIntrvl pollDateTime
    const QStringList masks = QStringList() << " hh:mm:ss.zzz" << " hh:mm:ss.zzz" << " hh:mm" << " hh:mm";

    for(int i = 0, imax = answr.listRows.size(), jMax = masks.size(); i < imax; i++){
        QStringList l = answr.listRows.at(i);

        const quint8 pollCode = l.at(2).toUInt();
        const bool convert2utc = !(pollCode == POLL_CODE_READ_END_DAY || pollCode == POLL_CODE_READ_END_MONTH || pollCode == POLL_CODE_METER_STATUS);

        for(int j = 0; j < jMax; j++){
            const int c = colsDt.at(j);
            const QDateTime dt = QDateTime::fromMSecsSinceEpoch(l.at(c).toLongLong(0, 16));
            if(!dt.isValid())
                continue;

            l.replace(c, convert2utc ? dt.toString(dateMask + masks.at(j)) : dt.toLocalTime().toString(dateMask + masks.at(j)));
        }
        list.append(l);
    }

    emit setZbyratorTasksPageSett(list, answr.col2data, TableHeaders::getColNamesZbyratorTasks().split(","), answr.header, !answr.header.isEmpty());

}
