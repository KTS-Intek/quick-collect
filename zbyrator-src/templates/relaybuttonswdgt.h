#ifndef RELAYBUTTONSWDGT_H
#define RELAYBUTTONSWDGT_H

#include <QWidget>
#include <QList>
#include <QPushButton>
#include <QLabel>
#include <QTableView>

namespace Ui {
class RelayButtonsWdgt;
}

class RelayButtonsWdgt : public QWidget
{
    Q_OBJECT

public:
    explicit RelayButtonsWdgt(QWidget *parent = nullptr);
    ~RelayButtonsWdgt();

    QList<QPushButton *> gimmeYourButtons();

    bool gimmeYourEnabled();



    void createSelectionChecker(QTableView *lastTv);

signals:
    void onDoRelayOperationSelected(quint8 operation);


    void lockActions(bool disable);


public slots:
    void setButtonsEnable(bool enable);

    void lockButtons(bool disable);

    void onWdgtLock(bool disable);

    void sendActLock(const bool &isWdgtDisabled, const bool &isButtonDisabled);

private slots:
    void on_pbRead_clicked();

    void on_pbLoadOn_clicked();

    void on_pbLoadOff_clicked();

    void on_pbLoadOff_2_clicked();

    void on_pbLoadOn_2_clicked();

    void doRelayOperationSelected(const quint8 &operation);


private:
    Ui::RelayButtonsWdgt *ui;
};

#endif // RELAYBUTTONSWDGT_H
