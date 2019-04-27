#ifndef STARTPOLLSETMETERMODELDIALOG_H
#define STARTPOLLSETMETERMODELDIALOG_H

#include <QDialog>
#include <QHash>


///[!] guisett-shared-core
#include "src/nongui/classmanagermeterinfo.h"


namespace Ui {
class StartPollSetMeterModelDialog;
}

class StartPollSetMeterModelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartPollSetMeterModelDialog(const QHash<QString, ClassManagerMeterInfo::MeterNi2info> &hashMeterNi2info, const QStringList &nis, const QStringList &models, QWidget *parent = nullptr);
    ~StartPollSetMeterModelDialog();

    QHash<QString, ClassManagerMeterInfo::MeterNi2info> hashMeterNi2info;
    QStringList nis;
    QStringList models;

    QStringList getOneRow(const QString &ni, const int &maxcols, QStringList &nosettnis);


signals:
    void setNi2model(QStringList modelnis);


    void killCbxDelegate();

private slots:
    void initThisPage();

    void on_buttonBox_accepted();

    void acceptSlot();

    void sendSett();

private:
    Ui::StartPollSetMeterModelDialog *ui;
};

#endif // STARTPOLLSETMETERMODELDIALOG_H
