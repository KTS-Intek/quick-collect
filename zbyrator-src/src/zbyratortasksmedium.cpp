#include "zbyratortasksmedium.h"

#include <QDateTime>
#include <QTimer>


///[!] matilda-bbb-serverside-shared
#include "matilda-bbb-src/decoders/decodematildaprotocolv5helper.h"


///[!] guisett-shared-core
#include "src/nongui/processtasktablereadout.h"


ZbyratorTasksMedium::ZbyratorTasksMedium(QObject *parent) : QObject(parent)
{


}


void ZbyratorTasksMedium::onThreadStarted()
{
    connect(this, &ZbyratorTasksMedium::onTaskTableChanged, this, &ZbyratorTasksMedium::sendTaskTableLater);

    QTimer *t = new QTimer(this);
    t->setSingleShot(true);
    t->setInterval(1111);
    connect(this, SIGNAL(startTmrSendTaskTable(int)), t, SLOT(start(int)));
    connect(t, &QTimer::timeout, this, &ZbyratorTasksMedium::onTmrSendTaskTableLater);

    emit startTmrSendTaskTable(5555);

}



void ZbyratorTasksMedium::sendTaskTableLater()
{
    emit startTmrSendTaskTable(3333);

}

void ZbyratorTasksMedium::onTmrSendTaskTableLater()
{
    try2sendTaskTable();
}

void ZbyratorTasksMedium::try2sendTaskTable()
{


    QStringList header;
    const QStringList list = DecodeMatildaProtocolV5Helper::preparyCacheTaskTable(header, 0, 0, QList<quint8>(), QList<quint8>(), QStringList());

    FramedHashAnswr answr;
    for(int i = 0, imax = list.size(); i < imax; i++){
        answr.listRows.append(list.at(i).split("|"));
    }
    UCTaskTableState tasks;
    tasks.tasksl.append(ProcessTaskTableReadout::getTaskList(answr));
    tasks.validator.allDataIsReceived = true;
    tasks.validator.dtlastupdate = QDateTime::currentDateTimeUtc();

    emit setUCTaskTableStateMeter(tasks);
}


