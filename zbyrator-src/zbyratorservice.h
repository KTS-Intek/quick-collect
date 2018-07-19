#ifndef ZBYRATORSERVICE_H
#define ZBYRATORSERVICE_H

#include "src/shared/matildaconfwidget.h"

namespace Ui {
class ZbyratorService;
}

class ZbyratorService : public MatildaConfWidget
{
    Q_OBJECT

public:
    explicit ZbyratorService(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);
    ~ZbyratorService();

public slots:
    void initPage();

private slots:
    void on_pbOpenDa_clicked();

private:
    Ui::ZbyratorService *ui;
};

#endif // ZBYRATORSERVICE_H
