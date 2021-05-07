#ifndef METERSDATETIMEBUTTONSWDGT_H
#define METERSDATETIMEBUTTONSWDGT_H

#include <QWidget>
#include <QList>
#include <QPushButton>
#include <QLabel>
#include <QTableView>


namespace Ui {
class MetersDateTimeButtonsWdgt;
}

class MetersDateTimeButtonsWdgt : public QWidget
{
    Q_OBJECT

public:
    explicit MetersDateTimeButtonsWdgt(QWidget *parent = nullptr);
    ~MetersDateTimeButtonsWdgt();


    QList<QPushButton *> gimmeYourButtons();

    bool gimmeYourEnabled();



    void createSelectionChecker(QTableView *lastTv);



signals:
    void onStartOperation(bool selectedOnly, quint8 operation);

    void lockActions(bool disable);

public slots:
    void setButtonsEnable(bool enable);

    void lockButtons(bool disable);

    void onWdgtLock(bool disable);

    void sendActLock(const bool &isWdgtDisabled, const bool &isButtonDisabled);


private slots:

    void on_pbCorrectionAll_clicked();

    void on_pbRead_clicked();

    void on_pbWrite_clicked();

private:
    Ui::MetersDateTimeButtonsWdgt *ui;

    void startOperation(const bool &selectedOnly, const quint8 &operation);

};

#endif // METERSDATETIMEBUTTONSWDGT_H
