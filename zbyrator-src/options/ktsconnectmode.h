#ifndef KTSCONNECTMODE_H
#define KTSCONNECTMODE_H

#include "src/shared/referencewidgetclass.h"

namespace Ui {
class KtsConnectMode;
}

class KtsConnectMode : public ReferenceWidgetClass
{
    Q_OBJECT

public:
    explicit KtsConnectMode(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);
    ~KtsConnectMode();

private:
    Ui::KtsConnectMode *ui;
};

#endif // KTSCONNECTMODE_H
