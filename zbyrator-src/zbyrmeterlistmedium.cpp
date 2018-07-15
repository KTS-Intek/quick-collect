#include "zbyrmeterlistmedium.h"
#include "src/widgets/tableheaders.h"
#include "src/zbyrator-v2/myucmmeters.h"
#include "src/meter/zbyratorfilesetthelper.cpp"
#include "src/matilda/matildaprotocolhelper.h"
#include <QDebug>
#include <QTimer>


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

void ZbyrMeterListMedium::onAllMeters(UniversalMeterSettList allMeters)
{
    if(allMeters.isEmpty())
        return;
    QVariantHash h;
    h.insert("lHeader", TableHeaders::getColNamesMeterList().split(","));
    h.insert("hasHeader", true);

//    const int icoCol = h.value("ico", -1).toInt();
//    const QStringList icoList = h.value("icos").toStringList();

//    QStringList listModelSnNiVrsnEnrg
    QVariantList meters = h.value("meters").toList();
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
        meters.append(l);
    }
    h.insert("meters", meters);
    emit setPageSett(h);
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
}

//---------------------------------------------------------------------
