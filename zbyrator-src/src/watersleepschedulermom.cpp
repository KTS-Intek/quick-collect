#include "watersleepschedulermom.h"


///[!] matilda-bbb-serverside-shared
#include "matilda-bbb-src/decoders/decodematildaprotocolv5.h"


///[!] meters-shared
#include "zbyrator-src/protocol5togui.h"

WaterSleepSchedulerMom::WaterSleepSchedulerMom(GuiHelper *gHelper, QWidget *parent) : WaterSleepScheduler(gHelper, parent)
{

    connect(this, &WaterSleepSchedulerMom::onSettingsChanged, this, &WaterSleepSchedulerMom::onSettChangedSlot);
}

void WaterSleepSchedulerMom::onSettChangedSlot()
{
    bool ok;
    QString mess;
    const QVariantHash h;// = Protocol5toGUI::getProfLinesFromTableCell(getPageSett(ok, mess, false));

//    QVariant ClassManagerV5::preparyCOMMAND_WRITE_WMETER_LAST_PROFILES(const QVariant &dataVar)


    IneedMoreTimeObject *timeObject = new IneedMoreTimeObject(this);
    LastMatildaObjSett lastObjSett;
    connect(this, SIGNAL(reloadSavedSleepProfiles()), timeObject, SLOT(deleteLater()));
    DecodeMatildaProtocolV5(timeObject, lastObjSett, this).saveWaterMetersProfilesSmpl(h.value("lv").toList());

    QTimer::singleShot(111, this, SIGNAL(reloadSavedSleepProfiles()));
}
