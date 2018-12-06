#include "watersleepschedulesaver.h"
#include "gui-src/settloader.h"
#include "src/zbyrator-v2/watermeterhelper.h"


QVariant WaterSleepScheduleSaver::getDefaultProfileVar()
{
    return QVariant::fromValue(getDefaultProfile());
}

QVariantHash WaterSleepScheduleSaver::getDefaultProfile()
{//only one sleep profile
    return WaterMeterHelper::getDefSleepOneProfile();
}

QVariantHash WaterSleepScheduleSaver::getDefaultSett()
{//name to default one sleep profile
    return WaterMeterHelper::getDefSleepSettt();
}

QVariantHash WaterSleepScheduleSaver::getSavedSett()
{
    const QVariantHash hsp = SettLoader::loadSett(SETT_METER_WATER_SLEEP).toHash();
    return hsp.isEmpty() ? getDefaultSett() : hsp;
}

void WaterSleepScheduleSaver::addNewProfile(const QString &name, const QVariantHash &profile)
{
    QVariantHash h = getSavedSett();
    h.insert(name, profile);
    SettLoader::saveSett(SETT_METER_WATER_SLEEP, h);
}

void WaterSleepScheduleSaver::removeOneProfile(const QString &name)
{
    QVariantHash h = getSavedSett();
    h.remove(name);
    SettLoader::saveSett(SETT_METER_WATER_SLEEP, h);
}

QMap<QString, QString> WaterSleepScheduleSaver::getSavedSettMap()
{
    const QVariantHash h = getSavedSett();
    const QList<QString> lk = h.keys();
    QMap<QString,QString> map;

    for(int i = 0, imax = lk.size(); i < imax; i++)
        map.insert(lk.at(i), WaterMeterHelper::oneProfile2lineSmpl(h.value(lk.at(i)).toHash()));
    return map;
}
