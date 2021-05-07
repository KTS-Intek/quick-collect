#ifndef RELAYWDGT_H
#define RELAYWDGT_H


///[!] widgets-shared
#include "gui-src/wdgt/referencewidgetclassgui.h"

///[!] quick-collect
#include "zbyrator-src/templates/relaybuttonswdgt.h"


///[!] widgets-meters
#include "dataconcetrator-pgs/emeterlistwdgt.h"


class RelayWdgt : public ReferenceWidgetClassGui
{
    Q_OBJECT

public:
    explicit RelayWdgt(GuiHelper *gHelper, QWidget *parent = 0);

    void replaceHeaderRoles4map(QStringList &heaaderroles, int &colKey, int &colPos);

    void updateMapGroupingSettings();


    QString updatePageContent(QString &errorStr);


    MTableFullHouse fromUCEMeterSettings(const UCEMeterSettings &settings);


    QStringList getHeader();



    QStringList getAvRelayStatuses();

    QStringList getSelectedNIs();
    QStringList getVisibleNIs();

signals:

    void updateRelayStatusTmr();
    void stopTmrRelyaStatusTmr();



    void command4dev(quint16 command, QVariantMap mapArgs);//pollCode args


    void setLastPageId(QString name);

    void lockButtons(bool disable);


public slots:
    void clearPage();

    void onUCEMeterSettingsChanged(UCEMeterSettings settings);


    void onUCEMeterRelayStateChanged(UCEMeterRelayState info);

    void getLastRelayStateSmart(const UCDataState &validator);
    void getLastRelayState();
    void updateRelayStatus();


//    void setPageSett(const MyListStringList &listRows, const QVariantMap &col2data, const QStringList &headerH, const QStringList &header, const bool &hasHeader);




//    void meterRelayStatus(QString ni, QDateTime dtLocal, QString stts, QString stts2, QString icostts, QString icostts2);
    void meterRelayStatus(QString ni, QDateTime dtLocal, quint8 mainstts, quint8 secondarystts);




//    void onButtonLock(bool disable); what is that ?


    void doRelayOperationSelected(const quint8 &operation);

private slots:

    void initPage();


    void onTvTableCustomContextMenuRequested(const QPoint &pos);


    void onPbReadAll_clicked();



private:

    void createTopWidget();

    void doRelayOperation(const QStringList &listni, const quint8 &operation);


    QList<QAction*> getRelayActions();

//    //this is from EMeterListWdgt
    EMeterListWdgt::LastRelayUpdate lRelayUpdate;


    QString lastDateTimeMask;

    RelayButtonsWdgt *buttonsWidget;
};

#endif // RELAYWDGT_H
