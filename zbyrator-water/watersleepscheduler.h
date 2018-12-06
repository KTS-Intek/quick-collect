#ifndef WATERSLEEPSCHEDULER_H
#define WATERSLEEPSCHEDULER_H

#include "gui-src/referencewidgetclass.h"
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

    QVariantHash getPageSett(bool &ok, QString &mess, const bool &is4copy);

signals:
    void onReloadAllMeters();
    void setLastPageId(QString name);
    void command4dev(quint16 command, QVariantMap mapArgs);//pollCode args


    void setProfile(QVariantHash profile);

    void startShowCurrProfileTmr();


    void reloadSavedSleepProfiles();




    void setEditNiPageSett(QString ni, QString profName, QString profLineSmpl);

    void setMaps(QMap<QString,QString> ni2model, QMap<QString,QString> ni2lastProfile);



public slots:
    void initPage();

    void clearPage();


    void setPageSett(const QVariantHash &h);

#ifdef IS_ZBYRATOR

    void setPageSett(const MyListStringList &listRows, const QVariantMap &col2data, const QStringList &headerH, const QStringList &header, const bool &hasHeader);
#endif

    void waterMeterSchedulerStts(QString ni, QDateTime dtLocal, QString stts, QVariantHash sheduler);


    void updateSleepProfilesSett();


    void setSelectedCount(int selectedItems);

    void openEditWidget(const bool &editMode);

    void onSettReceivedWithData();

private slots:
    void on_tbAdd_clicked();

    void on_tvTable_customContextMenuRequested(const QPoint &pos);

#ifdef IS_ZBYRATOR
    void startOperation(const QStringList &listni, const quint8 &pollCode);

    void onPbReadAll_clicked();


    void on_pbRead_clicked();
#endif
    void on_pbWrite_clicked();

    void onTvTable_clicked(const QModelIndex &index);

    void onShowCurProfileTmr();
    void on_tbEdit_clicked();




    void checkCreateAddWdgt();
    void createAddWdgt(const QString &ni, const QString &profName, const QString &profLineSmpl);

    void sendMeNewMaps();
    void onAddNi(QString ni, QString profileName, QString proflineSmpl);

    void on_tvTable_doubleClicked(const QModelIndex &index);

    void deleteSelected();
private:
    Ui::WaterSleepScheduler *ui;

    void updatetSleepProfiles(const QVariantHash &profileName2schedule);


    QString lastDateTimeMask;


};

#endif // WATERSLEEPSCHEDULER_H
