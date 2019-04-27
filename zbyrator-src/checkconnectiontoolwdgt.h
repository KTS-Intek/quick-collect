#ifndef CHECKCONNECTIONTOOLWDGT_H
#define CHECKCONNECTIONTOOLWDGT_H

#include "gui-src/referencewidgetclass.h"

namespace Ui {
class CheckConnectionToolWdgt;
}

class CheckConnectionToolWdgt : public ReferenceWidgetClass
{
    Q_OBJECT

public:
    explicit CheckConnectionToolWdgt(GuiHelper *gHelper, QWidget *parent = 0);
    ~CheckConnectionToolWdgt();

signals:
    void lockButtons(bool disable);


public slots:
    void initPage();

private slots:
    void on_pushButton_clicked();

private:
    Ui::CheckConnectionToolWdgt *ui;
};

#endif // CHECKCONNECTIONTOOLWDGT_H
