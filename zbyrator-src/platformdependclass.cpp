#include "platformdependclass.h"
#include "src/matilda/moji_defy.h"
#include "src/meter/definedpollcodes.h"
#include "src/matilda/matildalimits.h"

//========================================================================
PlatformDependClass::PlatformDependClass(QObject *parent) : QObject(parent)
{

}
//========================================================================
quint16 PlatformDependClass::getFirstCommand()
{
    return getListCommand().first();

}
//========================================================================
quint16 PlatformDependClass::getLastCommand()
{
    return getListCommand().last();

}
//========================================================================
QHash<QString, quint16> PlatformDependClass::getPlatformCommands()
{
    const QStringList lname = getListCommandName();
    const QList<quint16> lcomm = getListCommand();
    QHash<QString,quint16> h;
    for(int i = 0, iMax = lname.size(), iMax2 = lcomm.size(); i < iMax2; i++){
        h.insert( (i < iMax) ? lname.at(i) : QString::number(lcomm.at(i)), lcomm.at(i)  );
    }
    return h;
}
//========================================================================
QString PlatformDependClass::getMan()
{
    return tr("Device: emulator, %1\n"
              "Level: 0\n"
              "Keys\n"
              "-t \t TCP destination host and port, format <IPv4>:<Port> or [<IPv6>]:<Port>, <Port> value can be omitted, default value 8989.\n"
              "-s \t Serial port: must be valid port name\n"
              "-d \t Custom date time, mask yyyy-MM-dd hh:mm\n"
              "-g \t Depth interval, 0 < g < %2\n"
              "-n \t space separeted NI list. Add model: <model>&<NI>. Add password: <NI>@<password>. Add model and password <model>&<NI>@<password>. If meter NI exists in device memory model and password can be omitted.\n"
              "-m \t Max. tariff. By default 3.\n"
              "-e \t Coma separeted energy keys. Default (A+,A-,R+,R-).\n"
              "-i \t Ignore existing poll data. Default: not ignored.\n"
              "-a \t Enable async mode. Default: disabled.\n").arg(DEV_POLL_EMULATOR_L1).arg(MAX_GLYBYNA);
}
//========================================================================
QStringList PlatformDependClass::getListCommandName()
{
    return tr("Stop All;Network Reset;Meter Journal;Voltage;Power;Current values;End of day;End of month;Direct Access").split(";");

}
//========================================================================
QList<quint16> PlatformDependClass::getListCommand()
{
    QList<quint16> l;
    l.append(POLL_CODE_KILL_PRIORITY_TASKS);
    l.append(POLL_CODE_RESET_NETWORK);
    l.append(POLL_CODE_METER_STATUS);
    l.append(POLL_CODE_READ_VOLTAGE);
    l.append(POLL_CODE_READ_POWER);
    l.append(POLL_CODE_READ_TOTAL);
    l.append(POLL_CODE_READ_END_DAY);
    l.append(POLL_CODE_READ_END_MONTH);
    l.append(ZBRTR_OPEN_DIRECT_ACCESS);
    return l;
}
//========================================================================
