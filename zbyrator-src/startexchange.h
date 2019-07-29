#ifndef STARTEXCHANGE_H
#define STARTEXCHANGE_H

///[!] widgets-shared
#include "gui-src/matildaconfwidget.h"

///[!] quick-collect
#include "zbyrator-src/zbyrmeterlistmedium.h"

namespace Ui {
class StartExchange;
}

class StartExchange : public MatildaConfWidget
{
    Q_OBJECT

public:
    explicit StartExchange(GuiHelper *gHelper, QWidget *parent = 0);
    ~StartExchange();
    GuiHelper *guiHelper;

    bool localPbReadLocker;
    bool lockByCommandTmp;

    ZbyrMeterListMedium *metersListMedium;


public slots:
    void initPage();

    void unlockWdgts();


    void showWdgtByName(const QString &wdgtAccessibleName, const QString &wdgtTitle);

    void showWdgtByName(const QString &wdgtAccessibleName, const QString &wdgtTitle, const QIcon &itemIcon);

    void showLastWdgt();

    void updateScrollAreaHeight();

    void appendShowMessPlain(QString m);

    void addWdgt2stackWdgt(QWidget *w, const int &wdgtType, bool oneShot, QString actTxt, QString actIco);

    void lockPbRead(bool disable);


signals:
    void stopExchange();

    void onReloadAllMeters();

    void openEditMacProfileWdgt(bool isEditMode, QLineEdit *le );

    void pageReady();

    void killOldWdgtSingleShot();

    void lockButtons(bool disable);


    void onRequest2pollThese(QStringList nis, quint8 metertype);
    void onRequest2GetDataThese(QStringList nis, quint8 metertype);

private slots:

    void on_tbIfaceSett_clicked();


    void on_trDevOperation_clicked(const QModelIndex &index);

    void addWdgt2devStack(const QString &realPageName, const QString &wdgtTitle, const QIcon &itemIcon);

    void addWdgt2devStackExt(QWidget *w, const QString &realPageName, const QString &wdgtTitle, const QIcon &itemIcon);


    void on_pbStop_clicked();


    void on_pbRead_clicked();

    void checkPbReadEnabled();

    void onCommandStarted();
    void unlockTmpLock();

    void onSwDevicesCurrIndxChanged();


    void activateStartPollWdgt();


private:
    Ui::StartExchange *ui;
    MatildaConfWidget *currentMatildaWidget();

    MatildaConfWidget *createStartPagePoll(GuiHelper *gHelper, QWidget *parent = 0);

    MatildaConfWidget *createWaterSleepSchedulerWdgt(GuiHelper *gHelper, QWidget *parent = 0);

    MatildaConfWidget *createRelayWdgt(GuiHelper *gHelper, QWidget *parent = 0);

    MatildaConfWidget *createMetersDateTime(GuiHelper *gHelper, QWidget *parent = 0);

    MatildaConfWidget *createZbyrIfaceSett(GuiHelper *gHelper, QWidget *parent = 0);

    MatildaConfWidget *createStatisticWdgt(GuiHelper *gHelper, QWidget *parent = 0);

    MatildaConfWidget *createZbyratorTaskWdgt(GuiHelper *gHelper, QWidget *parent = 0);

    MatildaConfWidget *createZbyratorServiceWdgt(GuiHelper *gHelper, QWidget *parent = 0);

    MatildaConfWidget *createQuickDirectAccessWdgt(GuiHelper *gHelper, QWidget *parent = 0);

    MatildaConfWidget *createIfaceSett4groupsWdgt(GuiHelper *gHelper, QWidget *parent = 0);

    void showWdgtByNameData(const QString &named);


    QString lastWdgtAccessibleName;
    QString lastWdgtAccessibleNameInProcess;

};

#endif // STARTEXCHANGE_H
