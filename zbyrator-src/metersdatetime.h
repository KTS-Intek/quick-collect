#ifndef METERSDATETIME_H
#define METERSDATETIME_H

///[!] widgets-shared
#include "gui-src/wdgt/referencewidgetclassgui.h"


///[!] zbyrator-src
#include "templates/metersdatetimebuttonswdgt.h"


class MetersDateTime : public ReferenceWidgetClassGui
{
    Q_OBJECT

public:
    explicit MetersDateTime(GuiHelper *gHelper, QWidget *parent = 0);



    void replaceHeaderRoles4map(QStringList &heaaderroles, int &colKey, int &colPos);

    void updateMapGroupingSettings();


    QString updatePageContent(QString &errorStr);



    MTableFullHouse fromUCPollDeviceSettingsList(const QList<UCPollDeviceSettings> &settings);


    QStringList getHeader();




    QStringList getVisibleNIs();

    QStringList getSelectedNIs();

signals:

    void updateRelayStatusTmr();
    void stopTmrRelyaStatusTmr();


    void command4dev(quint16 command, QVariantMap mapArgs);//pollCode args

    void setLastPageId(QString name);
    void lockButtons(bool disable);



public slots:
    void clearPage();

//    void setPageSett(const MyListStringList &listRows, const QVariantMap &col2data, const QStringList &headerH, const QStringList &header, const bool &hasHeader);



    void meterDateTimeDstStatus(QString ni, QDateTime dtLocal, QString stts);


    void updateDateTimeDst();


    void onStartOperation(bool selectedOnly, quint8 operation);



    void onUCEMeterSettingsChanged(UCEMeterSettings settings);

    void onUCWMeterSettingsChanged(UCWMeterSettings settings);


private slots:

    void initPage();






    void onTvTableCustomContextMenuRequested(const QPoint &pos);

    void onPbReadAll_clicked();



private:
    void createTopWidget();

    void startOperation(const QStringList &listni, const quint8 &operation);

    QList<QAction*> getDateTimeActions();

    struct LastMetersDateTimeDst
    {
        qint64 msecpc;
        QString dtmeterstr;
        QString dstmeter;

        LastMetersDateTimeDst() {}
    };

    struct LastDateDstUpdate
    {
        QHash<QString, LastMetersDateTimeDst > dttable;
        QStringList checkednis;
        LastDateDstUpdate() {}
    } lDateTimeDst;




    QString lastDateTimeMask;

    MetersDateTimeButtonsWdgt  *buttonsWidget;
};

#endif // METERSDATETIME_H
