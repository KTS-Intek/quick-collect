#ifndef WATERSTARTPOLLSLEEPDLG_H
#define WATERSTARTPOLLSLEEPDLG_H

#include <QDialog>

namespace Ui {
class WaterStartPollSleepDlg;
}

class WaterStartPollSleepDlg : public QDialog
{
    Q_OBJECT

public:
    struct WtrSleepPollSett
    {
        bool sendSleepCommand;
        int secs;
        bool checkProfile;
        bool isValid;
        WtrSleepPollSett() : sendSleepCommand(true), secs(10), checkProfile(true), isValid(false) {}
    };

    explicit WaterStartPollSleepDlg(const WtrSleepPollSett &sett, QWidget *parent = nullptr);
    ~WaterStartPollSleepDlg();

signals:
    void setWaterSleepSett(bool sendSleepCommand, int secs, bool checkProfile);

private slots:
    void on_buttonBox_accepted();

    void on_cbxPwrManagement_clicked(bool checked);

private:
    Ui::WaterStartPollSleepDlg *ui;
};

#endif // WATERSTARTPOLLSLEEPDLG_H
