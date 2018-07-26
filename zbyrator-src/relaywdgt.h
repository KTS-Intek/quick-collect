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

    QVariant getPageSett4read(bool &ok, QString &mess);

signals:
    ///map 4 exchange stat
    void setNewDeviceModelEs(QVariantList vl);
    void showThisDeviceNIEs(QString ni);
    void showMapEs(QString lastLang);

    void onReloadAllMeters();

    void command4dev(quint16 command, QVariantMap mapArgs);//pollCode args


    void setLastPageId(QString name);
public slots:
    void clearPage();


    void setPageSett(const MyListStringList &listRows, const QVariantMap &col2data, const QStringList &headerH, const QStringList &header, const bool &hasHeader);


    void onModelChanged();

    void meterRelayStatus(QString ni, QDateTime dtLocal, QString stts);



private slots:

    void initPage();



    void on_tbShowList_clicked();

    void on_tbShowMap_clicked();

    void on_tvTable_customContextMenuRequested(const QPoint &pos);


    void onPbReadAll_clicked();

    void on_pbRead_clicked();

    void on_pbLoadOn_clicked();

    void on_pbLoadOff_clicked();

private:
    Ui::RelayWdgt *ui;

    void doRelayOperationSelected(const quint8 &operation);

    void doRelayOperation(const QStringList &listni, const quint8 &operation);

    bool isMapReady;
    QString lastDateTimeMask;

};

#endif // RELAYWDGT_H
