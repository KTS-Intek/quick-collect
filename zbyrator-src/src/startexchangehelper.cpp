#include "startexchangehelper.h"
#include "src/task/task_status.h"

StartExchangeHelper::StartExchangeHelper(QObject *parent) : QObject(parent)
{

}

QStringList StartExchangeHelper::getChListData(QStringList &listIcos, QStringList &chListNames)
{
    chListNames = tr("Poll;Water profiles;Relay;Queue;Statistic of the exchange;Date and time;Meter address;Check Connection Tool;Other;Interface").split(";");
    listIcos.clear();
    listIcos.append(":/katynko/svg/edit-table-insert-row-below.svg");
    listIcos.append(":/katynko/svg/chronometer.svg");
    listIcos.append(":/katynko/svg/format-disconnect-node.svg");
    listIcos.append(":/katynko/svg/format-list-ordered.svg");
    listIcos.append(":/katynko/svg/view-statistics.svg");
    listIcos.append(":/katynko/svg/chronometer.svg");
    listIcos.append(":/katynko/svg/address-book-new.svg");
    listIcos.append(":/katynko/svg/irc-voice.svg");
    listIcos.append(":/katynko/svg/irc-voice.svg");
    listIcos.append(":/katynko/svg/network-disconnect.svg");

    return getChList();

}

QStringList StartExchangeHelper::getChList()
{
    return QString("Poll;Scheduler for water meters;Relay;Queue;Statistic of the exchange;Date and time;Meter address;Check Connection Tool;Other;Interface").split(";");

}

QString StartExchangeHelper::getStts4rez(const quint8 &rez)
{
    QString s;
    switch(rez){
    case RD_EXCHANGE_DONE               : s = tr("ok)"); break;//     0//обмін завершено успішно
    case RD_EXCHANGE_ERROR              : s = tr("no answer("); break;//    1//нема відповіді на запит
    case RD_EXCHANGE_BAD_TASK           : s = tr("bad task("); break;//    2//завдання є не повним або пошкодженим
    case RD_EXCHANGE_NANSWR_MODEM       : s = tr("no answer("); break;//    3//нема відповіді від модема
    case RD_EXCHANGE_NANSWR_BUS         : s = tr("no answer("); break;//   4//нема відповіді від БУСу
    case RD_EXCHANGE_BADDATA_BUS        : s = tr("bad data"); break;//    5//отримано погані дані від БУСу
    case RD_EXCHANGE_CRDNTR_BUSY        : s = tr("busy"); break;//    6//координатор зайнятий іншою програмою
    case RD_EXCHANGE_NALL               : s = tr("not all"); break;//   7//обмін не завершено поки-що
    case RD_EXCHANGE_CANCELED           : s = tr("canceled"); break;//   8//задачу знято
    case RD_EXCHANGE_REQUEST2CHNGCNFG   : s = tr("config"); break;//   9//запит на зміну конфігурації, наприклад при детектуванні моделі
    case RD_EXCHANGE_DONE_WO_POLL       : s = tr("nothing to poll"); break;//   10//обміну не було, бо в ньому відсутня необхідність

    }

    return s.isEmpty() ? tr("unknown rez=%1").arg((int)rez) : s;
}
