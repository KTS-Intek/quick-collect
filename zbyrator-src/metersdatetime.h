#ifndef METERSDATETIME_H
#define METERSDATETIME_H

#include "src/shared/referencewidgetclass.h"

namespace Ui {
class MetersDateTime;
}

class MetersDateTime : public ReferenceWidgetClass
{
    Q_OBJECT

public:
    explicit MetersDateTime(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);
    ~MetersDateTime();


signals:
    ///map 4 exchange stat
    void setNewDeviceModelEs(QVariantList vl);
    void showThisDeviceNIEs(QString ni);
    void showMapEs(QString lastLang);


    void onReloadAllMeters();

public slots:
    void clearPage();

    void setPageSett(const MyListStringList &listRows, const QVariantMap &col2data, const QStringList &headerH, const QStringList &header, const bool &hasHeader);

    void onModelChanged();



private slots:

    void initPage();



    void on_tbShowList_clicked();

    void on_tbShowMap_clicked();


    void on_tvTable_customContextMenuRequested(const QPoint &pos);

private:
    Ui::MetersDateTime *ui;
    bool isMapReady;
};

#endif // METERSDATETIME_H
