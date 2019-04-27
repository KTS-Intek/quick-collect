#ifndef KTSCONNECTMODE_H
#define KTSCONNECTMODE_H

#include "gui-src/referencewidgetclass.h"

namespace Ui {
class KtsConnectMode;
}

class KtsConnectMode : public ReferenceWidgetClass
{
    Q_OBJECT

public:
    explicit KtsConnectMode(GuiHelper *gHelper, QWidget *parent = 0);
    ~KtsConnectMode();

private:
    Ui::KtsConnectMode *ui;
};

#endif // KTSCONNECTMODE_H
