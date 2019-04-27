#ifndef KTSCONNECTWDGT_H
#define KTSCONNECTWDGT_H

#include "gui-src/matildaconfwidget.h"

namespace Ui {
class KtsConnectWdgt;
}

class KtsConnectWdgt : public MatildaConfWidget
{
    Q_OBJECT

public:
    explicit KtsConnectWdgt(GuiHelper *gHelper, QWidget *parent = 0);
    ~KtsConnectWdgt();

private:
    Ui::KtsConnectWdgt *ui;
};

#endif // KTSCONNECTWDGT_H
