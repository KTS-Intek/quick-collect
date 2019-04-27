#ifndef SETMETERADDRESS_H
#define SETMETERADDRESS_H

#include "gui-src/matildaconfwidget.h"

namespace Ui {
class SetMeterAddress;
}

class SetMeterAddress : public MatildaConfWidget
{
    Q_OBJECT

public:
    explicit SetMeterAddress(GuiHelper *gHelper, QWidget *parent = 0);
    ~SetMeterAddress();

signals:
    void lockButtons(bool disable);


public slots:
    void initPage();

private slots:
    void on_pushButton_clicked();

private:
    Ui::SetMeterAddress *ui;
};

#endif // SETMETERADDRESS_H
