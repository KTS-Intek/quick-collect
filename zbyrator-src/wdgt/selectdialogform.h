#ifndef SELECTDIALOGFORM_H
#define SELECTDIALOGFORM_H

#include <QDialog>
#include "zbyrator-src/selectmeters4poll.h"

namespace Ui {
class SelectDialogForm;
}

class SelectDialogForm : public QDialog
{
    Q_OBJECT

public:
    explicit SelectDialogForm(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);
    ~SelectDialogForm();

    LastDevInfo *lDevInfo;

    GuiHelper *gHelper;

    GuiSett4all *gSett4all;


signals:
    void setPollSett(QDateTime dtFrom, QDateTime dtTo, quint8 pollCode);

    void command4dev(quint16 command, QString args);//pollCode args


public slots:
    void setPollSettSlot(QDateTime dtFrom, QDateTime dtTo, quint8 pollCode);

    void resetPollSett();

private slots:
    void createSelectWdgt();



private:
    Ui::SelectDialogForm *ui;


    struct LastPollSett{
        QDateTime dtFrom;
        QDateTime dtTo;
        quint8 pollCode;
    } lPollSett;
};

#endif // SELECTDIALOGFORM_H
