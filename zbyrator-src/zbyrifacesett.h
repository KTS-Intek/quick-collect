#ifndef ZBYRIFACESETT_H
#define ZBYRIFACESETT_H

#include "src/shared/matildaconfwidget.h"

namespace Ui {
class ZbyrIfaceSett;
}

class ZbyrIfaceSett : public MatildaConfWidget
{
    Q_OBJECT

public:
    explicit ZbyrIfaceSett(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);
    ~ZbyrIfaceSett();

private slots:
    void on_tbUpdateSerial_clicked();

private:
    Ui::ZbyrIfaceSett *ui;
};

#endif // ZBYRIFACESETT_H
