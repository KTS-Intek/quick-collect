#ifndef WATERSLEEPSCHEDULER_H
#define WATERSLEEPSCHEDULER_H

#include "src/shared/referencewidgetclass.h"
#include "editwaterprofile.h"

namespace Ui {
class WaterSleepScheduler;
}

class WaterSleepScheduler : public ReferenceWidgetClass
{
    Q_OBJECT

public:
    explicit WaterSleepScheduler(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);
    ~WaterSleepScheduler();


    QVariantHash getPageSett(bool &ok, QString &mess, const bool &is4copy);

signals:
    void onReloadAllMeters();


public slots:
    void initPage();

    void clearPage();

    void setPageSett(const MyListStringList &listRows, const QVariantMap &col2data, const QStringList &headerH, const QStringList &header, const bool &hasHeader);



private slots:
    void on_tbAdd_clicked();

private:
    Ui::WaterSleepScheduler *ui;
    EditWaterProfile *editWdgt;

};

#endif // WATERSLEEPSCHEDULER_H
