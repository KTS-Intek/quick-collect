#include "zbyrmeterlistmedium.h"
#include "src/widgets/tableheaders.h"
#include "src/zbyrator-v2/myucmmeterstypes.h"
#include "src/meter/zbyratorfilesetthelper.cpp"
#include "src/matilda/matildaprotocolhelper.h"
#include <QDebug>
#include <QTimer>
#include "zbyrator-src/src/zbyrtableheaders.h"
#include "src/matilda/standarditemmodelhelper.h"
#include "src/matilda/moji_defy.h"

#include "src/matilda/settloader.h"
#include "zbyrator-src/wdgt/compliterlistdialog.h"
#include "src/matilda/serialporthelper.h"
#include "src/shared/networkconverthelper.h"
#include "zbyrator-src/src/zbyratordatacalculation.h"
#include "zbyrator-src/src/startexchangehelper.h"

#include "zbyrator-src/src/zbyratordatabasemedium.cpp"
#include "zbyrator-water/src/watersleepschedulesaver.h"
#include "src/zbyrator-v2/watermeterhelper.h"

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
    createDatabaseMedium();
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

void ZbyrMeterListMedium::onAllMetersSlot(UniversalMeterSettList allMeters)
{
    if(allMeters.isEmpty())
        return;


    lastWaterSleepProfile = WaterSleepScheduleSaver::getSavedSett();

    QMap<quint8, QVariantMap> mapRowColDataByMeters;

    MyListStringList listRowsEl, listRowWater;
    QMap<quint8, int> mapRowCounter;

    QMap<quint8, UniversalMeterSettList> map2meters;
    for(int i = 0, imax = allMeters.size(); i < imax; i++){
        const UniversalMeterSett m = allMeters.at(i);

        if(true){
            UniversalMeterSettList l = map2meters.value(m.meterType);
            l.append(m);
            map2meters.insert(m.meterType, l);
        }
        QStringList ldata;
        QList<int> lcols;
        const QStringList onemeterrow = universalMeterSett2listRow(m, ldata, lcols);

        switch(m.meterType){
        case UC_METER_ELECTRICITY   : listRowsEl.append(onemeterrow)  ; break;
        case UC_METER_WATER         : listRowWater.append(onemeterrow); break;
        }

        int meterTypeRow = mapRowCounter.value(m.meterType, 0) ;
        mapRowCounter.insert( m.meterType,  meterTypeRow + 1);
        if(!ldata.isEmpty()){

            QVariantMap mapRowColData = mapRowColDataByMeters.value(m.meterType);
            for(int j = 0, jmax = ldata.size(); j < jmax; j++)
                mapRowColData.insert(QString("%1;%2").arg(meterTypeRow).arg(lcols.at(j)), ldata.at(j));
            mapRowColDataByMeters.insert(m.meterType, mapRowColData);
        }
    }

    if(true){
        const QStringList headerh = TableHeaders::getColNamesMeterList().split(",");
        emit setElectricityMeterListPageSett(listRowsEl, mapRowColDataByMeters.value(UC_METER_ELECTRICITY), headerh, StandardItemModelHelper::getHeaderData(headerh.size()), true);
    }

    if(true){
        const QStringList headerh = TableHeaders::getColNamesWaterMeterList().split(",");
        emit setWaterMeterListPageSett(listRowWater, mapRowColDataByMeters.value(UC_METER_WATER), headerh, StandardItemModelHelper::getHeaderData(headerh.size()), true);
    }
    const QList<quint8> lk = map2meters.keys();
    for(int i = 0, imax = lk.size(); i < imax; i++)
        emit onAddMeters(lk.at(i), map2meters.value(lk.at(i)), MyNi2model(), true);

    onElectricitylistOfMeters(allMeters, MyNi2model(), true);
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::meterElectricityModelChanged(QVariantList meters)
{
    emit onConfigChanged(MTD_EXT_CUSTOM_COMMAND_2, true);
    lastSaveMeterList.lastMeterList = meters;
    lastSaveMeterList.meterType = UC_METER_ELECTRICITY;
    emit startTmrSaveLater();

}

void ZbyrMeterListMedium::meterWaterModelChanged(QVariantList meters)
{
    emit onConfigChanged(MTD_EXT_CUSTOM_COMMAND_2, true);
    lastSaveMeterList.lastMeterList = meters;
    lastSaveMeterList.meterType = UC_METER_WATER;
    emit startTmrSaveLater();
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::onSaveLater()
{
    const quint8 meterType = lastSaveMeterList.meterType;
    const QString mess = ZbyratorFileSettHelper::saveMeters(meterType, lastSaveMeterList.lastMeterList);
    if(!mess.isEmpty())
        qDebug() << "ZbyrMeterListMedium mess " << mess;
    emit onConfigChanged(MTD_EXT_COMMAND_RELOAD_SETT, true);



    //MTD_EXT_COMMAND_RELOAD_SETT
    lastSaveMeterList.lastMeterList.clear();
    lastSaveMeterList.meterType = UC_METER_UNKNOWN;

//    doReloadListOfElectricityMeters();
//    doReloadListOfMeters(UC_METER_UNKNOWN);
    emit onReloadAllMeters2zbyrator();
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

void ZbyrMeterListMedium::command4devSlot(quint16 command, QVariantMap mapArgs)
{
    updateInterfaceSettings();

    emit command4dev(command, mapArgs);
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
    connect(this, &ZbyrMeterListMedium::onAddMeters     , c, &ZbyratorDataCalculation::onAlistOfMeters  );
    connect(this, &ZbyrMeterListMedium::appendMeterData , c, &ZbyratorDataCalculation::appendMeterData  );
    connect(this, &ZbyrMeterListMedium::onPollStarted   , c, &ZbyratorDataCalculation::onPollStarted    );
    connect(this, &ZbyrMeterListMedium::onUconStartPoll , c, &ZbyratorDataCalculation::onUconStartPoll  );

    connect(this, &ZbyrMeterListMedium::onMeterPollCancelled, c, &ZbyratorDataCalculation::onMeterPollCancelled);

    connect(c, &ZbyratorDataCalculation::appendData2model, this, &ZbyrMeterListMedium::appendData2model);
    connect(c, &ZbyratorDataCalculation::setLblWaitTxt   , this, &ZbyrMeterListMedium::setLblWaitTxt);
    connect(c, &ZbyratorDataCalculation::updateHashSn2meter, this, &ZbyrMeterListMedium::updateHashSn2meter);

    connect(c, &ZbyratorDataCalculation::setCOMMAND_READ_POLL_STATISTIC, this, &ZbyrMeterListMedium::setStatisticOfExchangePageSett);
    connect(this, &ZbyrMeterListMedium::onCOMMAND_READ_POLL_STATISTIC, c, &ZbyratorDataCalculation::onCOMMAND_READ_POLL_STATISTIC);
    t->start();

}

void ZbyrMeterListMedium::createDatabaseMedium()
{
    ZbyratorDatabaseMedium *m = new ZbyratorDatabaseMedium;
    QThread *t = new QThread;
    m->moveToThread(t);

    connect(this, SIGNAL(destroyed(QObject*)), t, SLOT(quit()) );
    connect(t, SIGNAL(started()), m, SLOT(onThreadStarted()) );
    connect(this, &ZbyrMeterListMedium::onAddMeters     , m, &ZbyratorDatabaseMedium::onAlistOfMeters);


    connect(this, SIGNAL(data2dbMedium(quint16,QVariant)), m, SLOT(data2matilda4inCMD(quint16,QVariant)));
    connect(this, SIGNAL(stopReadDatabase()), m, SLOT(stopOperationSlot()) );

    connect(this, &ZbyrMeterListMedium::setDateMask, m, &ZbyratorDatabaseMedium::setDateMask);
    connect(this, &ZbyrMeterListMedium::setDotPos, m, &ZbyratorDatabaseMedium::setDotPos);

    connect(m, SIGNAL(setPbReadEnableDisable(bool)), this, SIGNAL(setPbReadEnableDisable(bool)));

    connect(m, SIGNAL(appendDataDatabase(QVariantHash)), this, SIGNAL(appendDataDatabase(QVariantHash)));
    connect(m, SIGNAL(appendDataDatabaseMJ(QVariantHash)), this, SIGNAL(appendDataDatabaseMJ(QVariantHash)));

    connect(m, SIGNAL(setLblWaitTxtDatabase(QString)), this, SIGNAL(setLblWaitTxtDatabase(QString)));
    connect(m, SIGNAL(setLblWaitTxtDatabaseMj(QString)), this, SIGNAL(setLblWaitTxtDatabaseMj(QString)));
    t->start();


}

void ZbyrMeterListMedium::onAllStatHash(QStringList allstat)
{
    liststat = allstat;
}
//---------------------------------------------------------------------
void ZbyrMeterListMedium::onPollCodeChangedStat(QVariantHash hash)
{
    const QString pollStr = hash.value("c").toString();
    QString niStr = hash.value("n").toString();
    const int mode = hash.value("m", -1).toInt();


    if(mode < 0)
        niStr.clear();

    QStringList rezListStr;

    if(niStr.contains("'"))
        niStr.replace("'", " ");

    QRegExp regExp(niStr, Qt::CaseInsensitive, QRegExp::RegExp);

    const QStringList ls = liststat;
    if(!ls.isEmpty())
        rezListStr.append(ls.first());




//    QHash<QString, OneModemStat> hNi2allData;
//    QStringList lk2niAllData;

    for(int i = 1, iMax = ls.size(); i < iMax; i++){
        if(ls.at(i).split("\t", QString::SkipEmptyParts).isEmpty())
            continue;

        const QString niLine = ls.at(i).split("\t", QString::SkipEmptyParts).first();

        if(niStr.isEmpty() || (mode == 0 && niLine == niStr) || (mode == 1 && niLine.contains(regExp))){
            //    QString retVal(tr("Modem NI\tModel\tPoll Code\tTime\tMess. Count\tWrite/Read byte\tElapsed (s)\tLast retry\n"));
            if(pollStr.isEmpty()){
                rezListStr.append(ls.at(i));
            }else{
                const QStringList l = ls.at(i).split("\t", QString::SkipEmptyParts);
                if(l.size() < 5)
                    continue;
                if(l.at(2) == pollStr)
                    rezListStr.append(ls.at(i));

            }
        }
    }
    emit onCOMMAND_READ_POLL_STATISTIC(rezListStr);
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::onTaskCanceled(quint8 pollCode, QString ni, qint64 dtFinished, quint8 rez)
{

    if(rez == 0) // RD_EXCHANGE_DONE)
        return;
    Q_UNUSED(pollCode);
    const QString stts = StartExchangeHelper::getStts4rez(rez);

    emit appendAppLog(tr("%3: NI: %1, one task removed, rezult is %2").arg(ni).arg(stts).arg(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss")));

    const QDateTime dtLocal = QDateTime::fromMSecsSinceEpoch(dtFinished).toLocalTime();

    switch(lastPageMode){
    case 0: emit onMeterPollCancelled(ni, stts, dtFinished); break;//poll page
    case 1: emit waterMeterSchedulerStts(ni, dtLocal, stts, QVariantHash()); break;
    case 2: emit meterRelayStatus(ni, dtLocal, stts); break; //relay
    case 5: emit meterDateTimeDstStatus(ni, dtLocal, stts); break;
    }

}
//---------------------------------------------------------------------
void ZbyrMeterListMedium::setLastPageId(QString accsblName)
{
    //    return QString("Poll;Relay;Queue;Statistic of the exchange;Date and time;Meter address;Check Connection Tool;Other;Interface").split(";");
    lastPageMode = StartExchangeHelper::getChList().indexOf((accsblName));
}

void ZbyrMeterListMedium::onReloadAllMeters()
{
    QWidget *w = qobject_cast<QWidget *>(QObject::sender());
    if(w){
        const QString accessibleName = w->accessibleName();
        if(StartExchangeHelper::getChList().indexOf((accessibleName)) < 0)
            return;
        mapMeters2pages.insert(accessibleName, LastList2pages());
    }
    emit onReloadAllMeters2zbyrator();
}
//---------------------------------------------------------------------
QStringList ZbyrMeterListMedium::universalMeterSett2listRow(const UniversalMeterSett &m, QStringList &ldata, QList<int> &lcols)
{
    QStringList l;
    switch(m.meterType){
    case UC_METER_ELECTRICITY: l = universalMeterSett2listRowElectricity(m); break;
    case UC_METER_WATER      : l = universalMeterSett2listRowWater(m, ldata, lcols); break;
    }
    return l;

}
//---------------------------------------------------------------------
QStringList ZbyrMeterListMedium::universalMeterSett2listRowElectricity(const UniversalMeterSett &m)
{
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
    return l;
}
//---------------------------------------------------------------------
QStringList ZbyrMeterListMedium::universalMeterSett2listRowWater(const UniversalMeterSett &m, QStringList &ldata, QList<int> &lcols)
{
    QStringList l;
    l.append(m.model);
    l.append(m.sn);
    l.append(m.ni);
    l.append(m.memo);

    lcols.append(l.size());
    ldata.append(m.enrg);

    const QString profname = WaterMeterHelper::oneProfileFromLineName(lastWaterSleepProfile, m.enrg);
    l.append(profname.isEmpty() ? m.enrg : profname);//change enrg 2 sleep profile name


    l.append(m.pollEnbl ? "+" : "-");
    l.append(m.powerin);
    l.append(m.disableTimeCorrection ? "+" : "");
    l.append(m.coordinate);
    l.append(m.version);
    return l;
}
//---------------------------------------------------------------------
bool ZbyrMeterListMedium::metersChanged(QMap<QString, ZbyrMeterListMedium::LastList2pages> &mapMeters2pages, const QString &key, const LastList2pages &lastMeters2pagesL)
{
    if(mapMeters2pages.contains(key) && metersChanged(mapMeters2pages.value(key), lastMeters2pagesL)){
        mapMeters2pages.insert(key, lastMeters2pagesL);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------
bool ZbyrMeterListMedium::metersChanged(const ZbyrMeterListMedium::LastList2pages &lastMeters2pages, const ZbyrMeterListMedium::LastList2pages &lastMeters2pagesL)
{
     return (lastMeters2pages.listNI != lastMeters2pagesL.listNI || lastMeters2pages.mainParams != lastMeters2pagesL.mainParams);
}

//---------------------------------------------------------------------

void ZbyrMeterListMedium::onElectricitylistOfMeters(const UniversalMeterSettList &activeMeters, const MyNi2model &switchedOffMeters, const bool &checkOffMeters)
{
    const int activeMetersSize = activeMeters.size();
    const int switchedOffMetersSize = switchedOffMeters.size();


    QMap<QString, QStringList> relayPageL, dateTimePageL, waterProfiles;
    LastList2pages lastElectricityMeters2pagesL;

    LastList2pages lastWaterMeters2pagesL;

    LastList2pages lastMeters2pagesL;

    int pollOnMeters = 0;
    int pollOffMeters = switchedOffMetersSize;

    int meterElectricityActive = 0;
    int meterWaterActive = 0;
//    QStringList listNiNotchanged;

    QMap<quint8, QString> mapMetertype2ni;
    for(int i = 0; i < activeMetersSize; i++){

        const UniversalMeterSett m = activeMeters.at(i);
        if(checkOffMeters && !m.pollEnbl){
            pollOffMeters++;
            continue;
        }
        pollOnMeters++;
        const QString mainParms = QString("%1\t%2\t%3\t%4\t%5").arg(m.model).arg(m.version).arg(m.sn).arg(m.memo).arg(m.coordinate);//model version SN memo

        switch(m.meterType){
        case UC_METER_ELECTRICITY:{
            meterElectricityActive++;
            lastElectricityMeters2pagesL.listNI.append(m.ni);
            lastElectricityMeters2pagesL.mainParams.append(mainParms);
            relayPageL.insert(m.ni, addRelayRow(m));
            break;}

        case UC_METER_WATER:{
            meterWaterActive++;
            lastWaterMeters2pagesL.listNI.append(m.ni);
            lastWaterMeters2pagesL.mainParams.append(mainParms);
            waterProfiles.insert(m.ni, addWaterProfileRow(m));


            break;}
        }


        lastMeters2pagesL.listNI.append(m.ni);
        lastMeters2pagesL.mainParams.append(mainParms);
        dateTimePageL.insert(m.ni, addDateTimeRow(m));


//        if(lastMeters2pages.mainParams.contains(mainParms))
//            listNiNotchanged.append(m.ni);




        mapMetertype2ni.insertMulti(m.meterType, m.ni);

//        if(m.ni == UC_METER_ELECTRICITY)

           }

//    if(mapMeters2pages.contains("relay") && mapMeters2pages)

    if(metersChanged(mapMeters2pages, "Relay", lastElectricityMeters2pagesL))
        emit setRelayPageSett(getRowsList(relayPage, QStringList(), relayPageL, lastElectricityMeters2pagesL.listNI, meterElectricityActive), QVariantMap(), ZbyrTableHeaders::getRelayPageHeader(), StandardItemModelHelper::getHeaderData(7), true);


    if(metersChanged(mapMeters2pages, "Date and time", lastMeters2pagesL))
        emit setDateTimePageSett(getRowsList(dateTimePage, QStringList(), dateTimePageL, lastMeters2pagesL.listNI, pollOnMeters), QVariantMap(), ZbyrTableHeaders::getMeterDateTimePageHeader(), StandardItemModelHelper::getHeaderData(8), true);


    if(metersChanged(mapMeters2pages, "Scheduler for water meters", lastWaterMeters2pagesL))
        emit setWaterMeterSchedulerPageSett(getRowsList(dateTimePage, QStringList(), waterProfiles, lastWaterMeters2pagesL.listNI, meterWaterActive), QVariantMap(), ZbyrTableHeaders::getWaterMeterSchedulerPageHeader(), StandardItemModelHelper::getHeaderData(7), true);



//    if(checkOffMeters || lastMeters2pages.listNI != lastMeters2pagesL.listNI || lastMeters2pages.mainParams != lastMeters2pagesL.mainParams){


//        emit setDateTimePageSett(getRowsList(dateTimePage, listNiNotchanged, dateTimePageL, lastMeters2pagesL.listNI, pollOnMeters), QVariantMap(), ZbyrTableHeaders::getMeterDateTimePageHeader(), StandardItemModelHelper::getHeaderData(8), true);

//        lastMeters2pages.listNI = lastMeters2pagesL.listNI;
//        lastMeters2pages.mainParams = lastMeters2pagesL.mainParams;

//    }

    emit onUpdatedSavedList(pollOnMeters, pollOffMeters, meterElectricityActive, meterWaterActive);


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

QStringList ZbyrMeterListMedium::addWaterProfileRow(const UniversalMeterSett &m)
{
    QStringList dateTimeRow;
    dateTimeRow.append("");//time
    dateTimeRow.append("");//profile
    dateTimeRow.append(m.version.isEmpty() ? m.model : QString("%1, %2").arg(m.model).arg(m.version));
    dateTimeRow.append(m.sn);
    dateTimeRow.append(m.ni);
    dateTimeRow.append(m.memo);
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
