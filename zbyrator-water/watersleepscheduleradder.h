#ifndef WATERSLEEPSCHEDULERADDER_H
#define WATERSLEEPSCHEDULERADDER_H

#include "src/matilda-conf-wdgt/confpopupwdgt.h"
#include <QMap>

namespace Ui {
class WaterSleepSchedulerAdder;
}

class WaterSleepSchedulerAdder : public ConfPopupWdgt
{
    Q_OBJECT

public:
    explicit WaterSleepSchedulerAdder(QWidget *parent = nullptr);
    ~WaterSleepSchedulerAdder();

private slots:

    void on_pbAdd_clicked();

    void on_pbAddAndHide_clicked();

signals:
    void onAddNi(QString ni, QString profileName, QString proflineSmpl);


    void sendMeNewMaps();


public slots:

    void onTbClearPressed();

    void onTbUndoPressed();

    void clearPage();


    void addNi();

    void setPageSett(const QString &ni, const QString &profName, const QString &profLineSmpl);

    void setMaps(const QMap<QString,QString> &ni2model, const QMap<QString,QString> &ni2lastProfile);


    void updateModelLbl();



private:

    struct SavePageUndo
    {
        QString cbxNi;
        QString cbxProfile;
        QString cbxProfLineSmpl;//actv=
        QString hasSomeData;

        SavePageUndo() {}
        SavePageUndo(const QString &cbxNi, const QString &cbxProfile, const QString &cbxProfLineSmpl) : cbxNi(cbxNi), cbxProfile(cbxProfile), cbxProfLineSmpl(cbxProfLineSmpl) {}

    } pgsett;

    Ui::WaterSleepSchedulerAdder *ui;

    bool addNiProfile();

    QMap<QString,QString> ni2model;

};

#endif // WATERSLEEPSCHEDULERADDER_H
