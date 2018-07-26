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

    QVariant getPageSett4read(bool &ok, QString &mess);


signals:
    void onReloadAllMeters();
    void setLastPageId(QString name);
    void command4dev(quint16 command, QVariantMap mapArgs);//pollCode args


    void setProfile(QVariantHash profile);

    void startShowCurrProfileTmr();


    void setProfileEdit(QString name, QVariantHash profile);


public slots:
    void initPage();

    void clearPage();

    void setPageSett(const MyListStringList &listRows, const QVariantMap &col2data, const QStringList &headerH, const QStringList &header, const bool &hasHeader);


    void waterMeterSchedulerStts(QString ni, QDateTime dtLocal, QString stts, QVariantHash sheduler);

    void onSaveProfile(QString name, QVariantHash profile);

    void updateSleepProfilesSett();

    void deleteProfileName();

    void setSelectedCount(int selectedItems);


private slots:
    void on_tbAdd_clicked();

    void on_tvTable_customContextMenuRequested(const QPoint &pos);


    void startOperation(const QStringList &listni, const quint8 &pollCode);

    void onTvTable_clicked(const QModelIndex &index);

    void onShowCurProfileTmr();
    void on_tbEdit_clicked();

    void saveSleepProfiles();

    void onPbReadAll_clicked();

    void on_pbWrite_clicked();

    void on_pbRead_clicked();

private:
    Ui::WaterSleepScheduler *ui;

    void updatetSleepProfiles(const QVariantHash &profileName2schedule);


    QString lastDateTimeMask;

    QString profile2delete;

};

#endif // WATERSLEEPSCHEDULER_H
