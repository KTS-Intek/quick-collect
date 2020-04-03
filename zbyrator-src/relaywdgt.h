#ifndef RELAYWDGT_H
#define RELAYWDGT_H


///[!] widgets-shared
#include "gui-src/referencewidgetclass.h"


namespace Ui {
class RelayWdgt;
}

class RelayWdgt : public ReferenceWidgetClass
{
    Q_OBJECT

public:
    explicit RelayWdgt(GuiHelper *gHelper, QWidget *parent = 0);
    ~RelayWdgt();

    QVariant getPageSett4read(bool &ok, QString &mess);

    QStringList getAvRelayStatuses();

signals:
    ///map 4 exchange stat

    void setTableDataExt(const MPrintTableOut &table, const QStringList &header, const int &keycol);

    void setModelHeaderDataRoles(QString columnroles);// list joined with '\n'

    void showThisDeviceKeyValue(QString keyvalue);

    void setDefaultDataFilterSettings(QVariantMap maponeprofile, QString profilename);

    void showMapEs(QString lastLang);

    void onReloadAllMeters();

    void command4dev(quint16 command, QVariantMap mapArgs);//pollCode args


    void setLastPageId(QString name);

    void lockButtons(bool disable);
    void lockActions(bool disable);


public slots:
    void clearPage();


    void setPageSett(const MyListStringList &listRows, const QVariantMap &col2data, const QStringList &headerH, const QStringList &header, const bool &hasHeader);


    void onModelChanged();

//    void meterRelayStatus(QString ni, QDateTime dtLocal, QString stts, QString stts2, QString icostts, QString icostts2);
    void meterRelayStatus(QString ni, QDateTime dtLocal, quint8 mainstts, quint8 secondarystts);

    void showThisDev(QString ni);
    void showContextMenu4thisDev(QString ni);
    void showThisDevInSource(QString ni);


    void onWdgtLock(bool disable);
    void onButtonLock(bool disable);

    void sendActLock(const bool &isWdgtDisabled, const bool &isButtonDisabled);

private slots:

    void initPage();



    void on_tbShowList_clicked();

    void on_tbShowMap_clicked();

    void on_tvTable_customContextMenuRequested(const QPoint &pos);


    void onPbReadAll_clicked();

    void on_pbRead_clicked();

    void on_pbLoadOn_clicked();

    void on_pbLoadOff_clicked();

    void on_pbLoadOff_2_clicked();

    void on_pbLoadOn_2_clicked();

private:
    Ui::RelayWdgt *ui;

    void doRelayOperationSelected(const quint8 &operation);

    void doRelayOperation(const QStringList &listni, const quint8 &operation);


    QList<QAction*> getRelayActions();


    bool isMapReady;
    QString lastDateTimeMask;

};

#endif // RELAYWDGT_H
