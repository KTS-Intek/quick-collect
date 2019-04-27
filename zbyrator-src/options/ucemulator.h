#ifndef UCEMULATOR_H
#define UCEMULATOR_H

#include "gui-src/matildaconfwidget.h"

namespace Ui {
class UcEmulator;
}

class UcEmulator : public MatildaConfWidget
{
    Q_OBJECT

public:
    explicit UcEmulator(GuiHelper *gHelper, QWidget *parent = 0);
    ~UcEmulator();

private:
    Ui::UcEmulator *ui;
};

#endif // UCEMULATOR_H
