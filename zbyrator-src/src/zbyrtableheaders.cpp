#include "zbyrtableheaders.h"



ZbyrTableHeaders::ZbyrTableHeaders(QObject *parent) : QObject(parent)
{

}

QStringList ZbyrTableHeaders::getRelayPageHeader()
{
    return tr("Time,Main Relay,Second Relay,Meter,S/N,NI,Memo,Coordinate").split(",");

}

QStringList ZbyrTableHeaders::getMeterDateTimePageHeader()
{
    return tr("Computer,Meter time,Meter,S/N,NI,Memo,DST settings,Coordinate").split(",");
}

