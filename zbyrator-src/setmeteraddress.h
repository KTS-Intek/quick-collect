#ifndef SETMETERADDRESS_H
#define SETMETERADDRESS_H

#include "src/shared/matildaconfwidget.h"

namespace Ui {
class SetMeterAddress;
}

class SetMeterAddress : public MatildaConfWidget
{
    Q_OBJECT

public:
    explicit SetMeterAddress(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);
    ~SetMeterAddress();

private slots:
    void on_pushButton_clicked();

private:
    Ui::SetMeterAddress *ui;
};

#endif // SETMETERADDRESS_H
