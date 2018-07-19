#ifndef CHECKCONNECTIONTOOLWDGT_H
#define CHECKCONNECTIONTOOLWDGT_H

#include "src/shared/referencewidgetclass.h"

namespace Ui {
class CheckConnectionToolWdgt;
}

class CheckConnectionToolWdgt : public ReferenceWidgetClass
{
    Q_OBJECT

public:
    explicit CheckConnectionToolWdgt(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);
    ~CheckConnectionToolWdgt();

public slots:
    void initPage();

private slots:
    void on_pushButton_clicked();

private:
    Ui::CheckConnectionToolWdgt *ui;
};

#endif // CHECKCONNECTIONTOOLWDGT_H
