#ifndef WATERSCHEDULEEDITORPRO_H
#define WATERSCHEDULEEDITORPRO_H

#include "gui-src/referencewidgetclass.h"
#include "src/matilda/lastdevinfo.h"

namespace Ui {
class WaterScheduleEditorPro;
}

class WaterScheduleEditorPro : public ReferenceWidgetClass
{
    Q_OBJECT

public:
    explicit WaterScheduleEditorPro(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all,QWidget *parent = nullptr);
    ~WaterScheduleEditorPro();


signals:
    void startShowCurrProfileTmr();

    void setProfile(QVariantHash profile);
    void setProfileEdit(QString name, QVariantHash profile);

    void updateProfileSett();

public slots:
    void initPage();


    void onEditorReady2edit();

    void openAddEditWdgt(const int &row);

    void setEditProfileName(QString name);

    void setEditProfileFromMeter(QVariantHash h);

private slots:
    void createEditWdgt();

    void on_pbAdd_clicked();

    void on_pbDel_clicked();

    void onShowCurProfileTmr();

    void onTvTable_doubleclicked(const QModelIndex &index);

    void onSaveProfile(QString name, QVariantHash profile);

    void updateSleepProfilesSett();

    void deleteProfileName();

    void on_tvTable_customContextMenuRequested(const QPoint &pos);

private:
    Ui::WaterScheduleEditorPro *ui;
    QString profile2delete;
    QString editThisProfileName;
    QVariantHash editThisProfileSett;

};

#endif // WATERSCHEDULEEDITORPRO_H
