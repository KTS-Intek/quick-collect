#include "watersleepschedulesaver.h"
#include "src/matilda/settloader.h"



QVariant WaterSleepScheduleSaver::getDefaultProfileVar()
{
    return QVariant::fromValue(getDefaultProfile());
}

QVariantHash WaterSleepScheduleSaver::getDefaultProfile()
{
    return SettLoader::getDefSleepOneProfile();
}

QVariantHash WaterSleepScheduleSaver::getDefaultSett()
{
    return SettLoader::getDefSleepSettt();
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
