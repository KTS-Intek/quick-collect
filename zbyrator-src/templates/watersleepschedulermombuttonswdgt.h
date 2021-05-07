#ifndef WATERSLEEPSCHEDULERMOMBUTTONSWDGT_H
#define WATERSLEEPSCHEDULERMOMBUTTONSWDGT_H

#include <QWidget>
#include <QTableView>

namespace Ui {
class WaterSleepSchedulerMomButtonsWdgt;
}

class WaterSleepSchedulerMomButtonsWdgt : public QWidget
{
    Q_OBJECT

public:
    explicit WaterSleepSchedulerMomButtonsWdgt(QWidget *parent = nullptr);
    ~WaterSleepSchedulerMomButtonsWdgt();

    void createSelectionChecker(QTableView *lastTv);
signals:
    void onDoOperationSelected(quint8 operation);
    void lockActions(bool disable);

public slots:
    void setButtonsEnable(bool enable);

    void lockButtons(bool disable);

    void onWdgtLock(bool disable);

    void sendActLock(const bool &isWdgtDisabled, const bool &isButtonDisabled);



private slots:
    void on_pbRead_clicked();

    void on_pbWrite_clicked();


    void doOperationSelected(const quint8 &operation);

private:
    Ui::WaterSleepSchedulerMomButtonsWdgt *ui;
};

#endif // WATERSLEEPSCHEDULERMOMBUTTONSWDGT_H
