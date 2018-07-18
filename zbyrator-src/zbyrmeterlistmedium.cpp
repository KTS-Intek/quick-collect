#include "zbyrmeterlistmedium.h"
#include "src/widgets/tableheaders.h"
#include "src/zbyrator-v2/myucmmeterstypes.h"
#include "src/meter/zbyratorfilesetthelper.cpp"
#include "src/matilda/matildaprotocolhelper.h"
#include <QDebug>
#include <QTimer>
#include "zbyrator-src/src/zbyrtableheaders.h"
#include "src/matilda/standarditemmodelhelper.h"

#include "src/matilda/settloader.h"
#include "zbyrator-src/wdgt/compliterlistdialog.h"
#include "src/matilda/serialporthelper.h"
#include "src/shared/networkconverthelper.h"
#include "zbyrator-src/src/zbyratordatacalculation.h"

//---------------------------------------------------------------------

ZbyrMeterListMedium::ZbyrMeterListMedium(QObject *parent) : QObject(parent)
{
    QTimer *t = new QTimer(this);
    t->setSingleShot(true);
    t->setSingleShot(111);
    connect(this, SIGNAL(startTmrSaveLater()), t, SLOT(start())) ;
    connect(t, SIGNAL(timeout()), this, SLOT(onSaveLater()) );

    ifaceLoader = new IfaceSettLoader(this);

    createDataCalculator();

}

//---------------------------------------------------------------------

QVariantHash ZbyrMeterListMedium::getActiveIfaceSett() const
{
    QVariantHash h = getIfaceSett();

    QString s;
    switch(h.value("ifaceMode").toInt()){
    case 1: s = "uartTimeout uartBlockTimeout m2mTimeout m2mBlockTimeout"; break;
    case 2: s = "uartTimeout uartBlockTimeout tcpTimeout tcpBlcokTimeout"; break;
    default: s = "tcpTimeout tcpBlcokTimeout m2mTimeout m2mBlockTimeout"; break;
    }

    const QStringList l = s.split(" ", QString::SkipEmptyParts);
    for(int i = 0, imax = l.size(); i < imax; i++)
        h.remove(l.at(i));
    return h;
}

//---------------------------------------------------------------------

QVariantHash ZbyrMeterListMedium::getIfaceSett() const
{
    return SettLoader::loadSett(SETT_ZBRTR_IFACE_SETT).toHash();
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::onAllMeters(UniversalMeterSettList allMeters)
{
    if(allMeters.isEmpty())
        return;


//    QVariantHash h;
//    h.insert("lHeader", TableHeaders::getColNamesMeterList().split(","));
//    h.insert("hasHeader", true);

//    const int icoCol = h.value("ico", -1).toInt();
//    const QStringList icoList = h.value("icos").toStringList();

//    QStringList listModelSnNiVrsnEnrg
//    QVariantList meters = h.value("meters").toList();


    MyListStringList listRows;
    QMap<quint8, UniversalMeterSettList> map2meters;
    for(int i = 0, imax = allMeters.size(); i < imax; i++){
        const UniversalMeterSett m = allMeters.at(i);

        if(true){
            UniversalMeterSettList l = map2meters.value(m.meterType);
            l.append(m);
            map2meters.insert(m.meterType, l);
        }
        if(m.meterType != UC_METER_ELECTRICITY)
            continue;

        QStringList l;
        l.append(m.model);
        l.append(m.sn);
        l.append(m.ni);
        l.append(m.memo);
        l.append(m.passwd);
        l.append(m.pollEnbl ? "+" : "-");
        l.append(m.enrg);
        l.append(QString::number(m.tariff));
        l.append(m.coordinate);
        l.append(m.version);
        l.append(m.powerin);
        l.append(MeterOperations::transformerSett2line(m.transformer));
        l.append(m.disableTimeCorrection ? "+" : "");
        listRows.append(l);
    }
//    h.insert("meters", meters);
//    emit setMeterListPageSett(h);

    const QStringList headerh = TableHeaders::getColNamesMeterList().split(",");
    emit setMeterListPageSett(listRows, QVariantMap(), headerh, StandardItemModelHelper::getHeaderData(headerh.size()), true);

    //    void setRelayPageSett(MyListStringList listRows, QVariantMap col2data, QStringList headerH, QStringList header, bool hasHeader);

    const QList<quint8> lk = map2meters.keys();
    for(int i = 0, imax = lk.size(); i < imax; i++)
        emit onAddMeters(lk.at(i), map2meters.value(lk.at(i)), MyNi2model(), true);

    onElectricitylistOfMeters(allMeters, MyNi2model(), true);
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::meterModelChanged(QVariantList meters)
{
    emit onConfigChanged(MTD_EXT_CUSTOM_COMMAND_2, true);

    lastMeterList = meters;

    emit startTmrSaveLater();

}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::onSaveLater()
{
    const QString mess = ZbyratorFileSettHelper::saveElectricityMeters(lastMeterList);
    if(!mess.isEmpty())
        qDebug() << "ZbyrMeterListMedium mess " << mess;
    emit onConfigChanged(MTD_EXT_COMMAND_RELOAD_SETT, true);



    //MTD_EXT_COMMAND_RELOAD_SETT
    lastMeterList.clear();

    doReloadListOfElectricityMeters();
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::doReloadListOfElectricityMeters()
{
    doReloadListOfMeters(UC_METER_ELECTRICITY);
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::doReloadListOfMeters(quint8 meterType)
{
    emit sendMeAlistOfMeters(meterType);
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::onAlistOfMeters(quint8 meterType, UniversalMeterSettList activeMeters, MyNi2model switchedOffMeters)
{
    emit onAddMeters(meterType, activeMeters, switchedOffMeters, false);
    switch (meterType) {

    case UC_METER_ELECTRICITY: onElectricitylistOfMeters(activeMeters, switchedOffMeters, false); break;
    default:
        break;
    }
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::sendMeIfaceSett()
{
    emit setIfaceSett(getIfaceSett());
    emit setTcpClientCompliter(SettLoader::loadSett(SETT_ZBRTR_TCP_HISTORY).toStringList());

}
//---------------------------------------------------------------------
void ZbyrMeterListMedium::openTcpServerDlg(QLineEdit *le)
{
    openIpHistoryDlg(0, le);
}
//---------------------------------------------------------------------

void ZbyrMeterListMedium::openM2mDlg(QLineEdit *le)
{
    openIpHistoryDlg(1, le);
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::setNewSettings(QVariantHash h)
{
    SettLoader::saveSett(SETT_ZBRTR_IFACE_SETT, h);
}
//---------------------------------------------------------------------
void ZbyrMeterListMedium::command4devSlot(quint16 command, QString args)
{
  //reload interface settings
    updateInterfaceSettings();

    emit command4dev(command, args);
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::onListChanged(const QStringList &list, const int &tag)
{
    switch (tag) {
    case 0: SettLoader::saveSett(SETT_ZBRTR_TCP_HISTORY, list); emit setTcpClientCompliter(list); break;
    case 1:
    case 2: IfaceSettLoader::saveOnlyThisProfiles(list);   break;

    }
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::createDataCalculator()
{
    ZbyratorDataCalculation *c = new ZbyratorDataCalculation;
    QThread *t = new QThread;
    c->moveToThread(t);

    connect(this, SIGNAL(destroyed(QObject*)), t, SLOT(quit()) );
    connect(t, SIGNAL(started()), c, SLOT(onThreadStarted()) );
    connect(this, &ZbyrMeterListMedium::onAddMeters, c, &ZbyratorDataCalculation::onAlistOfMeters);
    connect(this, &ZbyrMeterListMedium::appendMeterData, c, &ZbyratorDataCalculation::appendMeterData);
    connect(this, &ZbyrMeterListMedium::onPollStarted, c, &ZbyratorDataCalculation::onPollStarted);
    connect(c, &ZbyratorDataCalculation::appendData2model, this, &ZbyrMeterListMedium::appendData2model);

    t->start();

}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::onElectricitylistOfMeters(const UniversalMeterSettList &activeMeters, const MyNi2model &switchedOffMeters, const bool &checkOffMeters)
{
    const int activeMetersSize = activeMeters.size();
    const int switchedOffMetersSize = switchedOffMeters.size();


    QMap<QString, QStringList> relayPageL, dateTimePageL;
    LastList2pages lastMeters2pagesL;

    int pollOnMeters = 0;
    int pollOffMeters = switchedOffMetersSize;
    QStringList listNiNotchanged;

    for(int i = 0; i < activeMetersSize; i++){

        const UniversalMeterSett m = activeMeters.at(i);
        if(checkOffMeters && !m.pollEnbl){
            pollOffMeters++;
            continue;
        }
        pollOnMeters++;

        const QString mainParms = QString("%1\t%2\t%3\t%4\t%5").arg(m.model).arg(m.version).arg(m.sn).arg(m.memo).arg(m.coordinate);//model version SN memo

        if(lastMeters2pages.mainParams.contains(mainParms))
            listNiNotchanged.append(m.ni);

        lastMeters2pagesL.listNI.append(m.ni);
        lastMeters2pagesL.mainParams.append(mainParms);
        relayPageL.insert(m.ni, addRelayRow(m));
        dateTimePageL.insert(m.ni, addDateTimeRow(m));

    }

    if(checkOffMeters || lastMeters2pages.listNI != lastMeters2pagesL.listNI || lastMeters2pages.mainParams != lastMeters2pagesL.mainParams){

        emit setRelayPageSett(getRowsList(relayPage, listNiNotchanged, relayPageL, lastMeters2pagesL.listNI, pollOnMeters), QVariantMap(), ZbyrTableHeaders::getRelayPageHeader(), StandardItemModelHelper::getHeaderData(7), true);

        emit setDateTimePageSett(getRowsList(dateTimePage, listNiNotchanged, dateTimePageL, lastMeters2pagesL.listNI, pollOnMeters), QVariantMap(), ZbyrTableHeaders::getMeterDateTimePageHeader(), StandardItemModelHelper::getHeaderData(8), true);

        lastMeters2pages.listNI = lastMeters2pagesL.listNI;
        lastMeters2pages.mainParams = lastMeters2pagesL.mainParams;

    }

    emit onUpdatedSavedList(pollOnMeters, pollOffMeters);


}

//---------------------------------------------------------------------

QStringList ZbyrMeterListMedium::addRelayRow(const UniversalMeterSett &m)
{
    QStringList relayRow;
    relayRow.append("");//datetime
    relayRow.append("");//relay
    relayRow.append(m.version.isEmpty() ? m.model : QString("%1, %2").arg(m.model).arg(m.version));
    relayRow.append(m.sn);
    relayRow.append(m.ni);
    relayRow.append(m.memo);
    relayRow.append(m.coordinate);
    return relayRow;
}

//---------------------------------------------------------------------

QStringList ZbyrMeterListMedium::addDateTimeRow(const UniversalMeterSett &m)
{
    QStringList dateTimeRow;
    dateTimeRow.append("");//datetime PC
    dateTimeRow.append("");//dateTime Meter
    dateTimeRow.append(m.version.isEmpty() ? m.model : QString("%1, %2").arg(m.model).arg(m.version));
    dateTimeRow.append(m.sn);
    dateTimeRow.append(m.ni);
    dateTimeRow.append(m.memo);
    dateTimeRow.append("");//DST profile
    dateTimeRow.append(m.coordinate);
    return dateTimeRow;

}

//---------------------------------------------------------------------

MyListStringList ZbyrMeterListMedium::getRowsList(QMap<QString, QStringList> &mapPage, const QStringList &listNiNotchanged, const QMap<QString, QStringList> &mapPageL, const QStringList listNI, const int &rowsCounter)
{
    MyListStringList listRows;
    for(int i = 0; i < rowsCounter; i++){
        const QString ni = listNI.at(i);
        listRows.append( (listNiNotchanged.contains(ni) && mapPage.contains(ni)) ? mapPage.value(ni) : mapPageL.value(ni));
    }

    mapPage = mapPageL;
    return listRows;
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::openIpHistoryDlg(const int &dlgMode, QLineEdit *le)
{
    QStringList ipListHistory, icoList;
    int colCount = 1;
    QString separ = "\t";

    switch(dlgMode){
    case 0: ipListHistory = SettLoader::loadSett(SETT_ZBRTR_TCP_HISTORY).toStringList(); separ = "\n"; break;
    case 1: ipListHistory = IfaceSettLoader::getProfileNames(icoList); colCount = 3; break;
    case 2: ipListHistory = IfaceSettLoader::getProfileNames(icoList); colCount = 3; break;

    }

    CompliterListDialog *dialog = new CompliterListDialog(ipListHistory, dlgMode, icoList, colCount, separ, true);

    if(le)
        connect(dialog, SIGNAL(selectedString(QString)), le, SLOT(setText(QString)) );

    connect(dialog, SIGNAL(onListChanged(QStringList,int)), SLOT(onListChanged(QStringList,int)) );
    dialog->exec();
    dialog->deleteLater();
}

bool ZbyrMeterListMedium::updateInterfaceSettings()
{
    const QVariantHash h = getIfaceSett();
    ifaceLoader->addThisDev2connPage(IfaceSettLoader::getProfileSett(h.value("m2mProfile").toString()));

//     QStringList listOfAutSerials, QString ipOrUartName, int tcpPortOrBaudRate, int timeoutSec, int blockTimeoutSec, QVariantHash profileSett
    QVariantMap interfaceSettings;

    interfaceSettings.insert("ifaceMode", h.value("ifaceMode"));

    QString keyTimeout, keyTimeoutBlock;
    switch(h.value("ifaceMode").toInt()){
    case 1 :{
        keyTimeout = "tcpTimeout";
        keyTimeoutBlock = "tcpBlcokTimeout";

        const HostNamePort host = NetworkConvertHelper::getHostAndPortFromLine(h.value("tcpHost").toString(), 8989);
        if(host.hostName.isEmpty()){
            emit showMess(tr("Can't find the host address"));
            return false;
        }

        interfaceSettings.insert("hostName", host.hostName);
        interfaceSettings.insert("port", host.port);


        break;}
    case 2 :{
        QVariantHash hash = IfaceSettLoader::getProfileSett(h.value("m2mProfile").toString());

        if(hash.isEmpty()){
            emit showMess(tr("Can't find the M2M profile"));
            return false;
        }

        hash.insert("t", h.value("m2mTimeout", 11000).toInt() / 1000 );

        interfaceSettings.insert("m2mProfile", hash);
        keyTimeout = "m2mTimeout";
        keyTimeoutBlock = "m2mBlockTimeout";
         break;}
    default:{
        const QStringList uarts = SerialPortHelper::uartList();

        if(uarts.isEmpty()){
            emit showMess(tr("Can't find any serial port"));
            return false;
        }

        if(h.value("uartManual").toBool() && !uarts.contains(h.value("lastUart").toString())){
            emit showMess(tr("Can't find the serial port"));
            return false;
        }

        if(h.value("uartManual").toBool()){
            if(!uarts.contains(h.value("lastUart").toString())){
                emit showMess(tr("Can't find the serial port"));
                return false;
            }
            interfaceSettings.insert("uart", h.value("lastUart").toString());
        }else{
            interfaceSettings.insert("uarts", uarts);
            if(uarts.contains(h.value("defUart").toString()))
                interfaceSettings.insert("uart", h.value("defUart").toString());
        }

        interfaceSettings.insert("baudRate", h.value("baudRate"));

        keyTimeout = "uartTimeout";
        keyTimeoutBlock = "uartBlockTimeout";
        break;}
    }

    interfaceSettings.insert("timeoutMsec", h.value(keyTimeout, 11000));
    interfaceSettings.insert("blockTimeout", h.value(keyTimeoutBlock, 300));

    emit setThisIfaceSett(interfaceSettings);
    emit setPollSaveSettings(3, 2, false, false, true, 11, 55, 33);

    return true;
}


//---------------------------------------------------------------------
