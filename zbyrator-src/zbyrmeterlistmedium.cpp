#include "zbyrmeterlistmedium.h"
#include "src/widgets/tableheaders.h"
#include "src/zbyrator-v2/myucmmeters.h"
#include "src/meter/zbyratorfilesetthelper.cpp"
#include "src/matilda/matildaprotocolhelper.h"
#include <QDebug>
#include <QTimer>
#include "zbyrator-src/src/zbyrtableheaders.h"
#include "src/matilda/standarditemmodelhelper.h"

#include "src/matilda/settloader.h"

//---------------------------------------------------------------------

ZbyrMeterListMedium::ZbyrMeterListMedium(QObject *parent) : QObject(parent)
{
    QTimer *t = new QTimer(this);
    t->setSingleShot(true);
    t->setSingleShot(111);
    connect(this, SIGNAL(startTmrSaveLater()), t, SLOT(start())) ;
    connect(t, SIGNAL(timeout()), this, SLOT(onSaveLater()) );
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
    for(int i = 0, imax = allMeters.size(); i < imax; i++){
        const UniversalMeterSett m = allMeters.at(i);
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
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::setNewSettings(QVariantHash h)
{
    SettLoader::saveSett(SETT_ZBRTR_IFACE_SETT, h);
}

void ZbyrMeterListMedium::command4devSlot(quint16 command, QString args)
{
  //reload interface settings


    emit command4dev(command, args);
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
