#ifndef WATERSLEEPSCHEDULERMOM_H
#define WATERSLEEPSCHEDULERMOM_H


///[!] widgets-meters
#include "zbyrator-water/watersleepscheduler.h"


///[!] quick-collect
#include "zbyrator-src/templates/watersleepschedulermombuttonswdgt.h"


class WaterSleepSchedulerMom : public WaterSleepScheduler
{
    Q_OBJECT
public:
    explicit WaterSleepSchedulerMom(GuiHelper *gHelper, QWidget *parent = nullptr);

    QStringList getSelectedNIs();

    QStringList getVisibleNIs();

    QHash<QString,QStringList> getSleepLines2nis(const QStringList &listNi, QStringList &lk);

signals:
    void setLastPageId(QString name);
    void command4dev(quint16 command, QVariantMap mapArgs);//pollCode args


    void reloadSavedSleepProfiles();

    void lockButtons(bool disable);


public slots:
    void onSettChangedSlot();

//    void waterMeterSchedulerStts(QString ni, QDateTime dtLocal, QString stts, QVariantHash sheduler, QString src);

    void onDoOperationSelected(quint8 operation);

    void startOperation(const QStringList &listni, const quint8 &pollCode);




private slots:
    void setupButtonsWidget();

private:
    void createTopWidget();

    WaterSleepSchedulerMomButtonsWdgt *buttonsWidget;


};

#endif // WATERSLEEPSCHEDULERMOM_H
