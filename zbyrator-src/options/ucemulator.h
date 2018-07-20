#ifndef UCEMULATOR_H
#define UCEMULATOR_H

#include "src/shared/matildaconfwidget.h"

namespace Ui {
class UcEmulator;
}

class UcEmulator : public MatildaConfWidget
{
    Q_OBJECT

public:
    explicit UcEmulator(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);
    ~UcEmulator();

private:
    Ui::UcEmulator *ui;
};

#endif // UCEMULATOR_H
