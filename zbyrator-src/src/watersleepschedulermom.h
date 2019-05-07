#ifndef WATERSLEEPSCHEDULERMOM_H
#define WATERSLEEPSCHEDULERMOM_H


///[!] widgets-meters
#include "zbyrator-water/watersleepscheduler.h"

class WaterSleepSchedulerMom : public WaterSleepScheduler
{
    Q_OBJECT
public:
    explicit WaterSleepSchedulerMom(GuiHelper *gHelper, QWidget *parent = nullptr);

signals:

public slots:
    void onSettChangedSlot();
};

#endif // WATERSLEEPSCHEDULERMOM_H
