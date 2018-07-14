#ifndef RELAYWDGT_H
#define RELAYWDGT_H

#include "src/shared/referencewidgetclass.h"

namespace Ui {
class RelayWdgt;
}

class RelayWdgt : public ReferenceWidgetClass
{
    Q_OBJECT

public:
    explicit RelayWdgt(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);
    ~RelayWdgt();

signals:
    ///map 4 exchange stat
    void setNewDeviceModelEs(QVariantList vl);
    void showThisDeviceNIEs(QString ni);
    void showMapEs(QString lastLang);

public slots:
    void clearPage();

    void setPageSett(const QVariantHash &h);

    void onModelChanged();



private slots:

    void initPage();



    void on_tbShowList_clicked();

    void on_tbShowMap_clicked();

private:
    Ui::RelayWdgt *ui;
    bool isMapReady;

};

#endif // RELAYWDGT_H
