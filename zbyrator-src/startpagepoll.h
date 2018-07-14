#ifndef STARTPAGEPOLL_H
#define STARTPAGEPOLL_H

#include "src/shared/matildaconfwidget.h"

namespace Ui {
class StartPagePoll;
}

class StartPagePoll : public MatildaConfWidget
{
    Q_OBJECT

public:
    explicit StartPagePoll(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);
    ~StartPagePoll();

private slots:
    void on_pbReadDb_clicked();

private:
    Ui::StartPagePoll *ui;
};

#endif // STARTPAGEPOLL_H
