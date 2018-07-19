#ifndef STARTEXCHANGE_H
#define STARTEXCHANGE_H

#include "src/shared/matildaconfwidget.h"
#include "zbyrator-src/zbyrmeterlistmedium.h"

namespace Ui {
class StartExchange;
}

class StartExchange : public MatildaConfWidget
{
    Q_OBJECT

public:
    explicit StartExchange(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);
    ~StartExchange();
    GuiHelper *guiHelper;

    ZbyrMeterListMedium *metersListMedium;
    static QStringList getChListData(QStringList &listIcos, QStringList &chListNames);


public slots:
    void initPage();

    void unlockWdgts();


    void showWdgtByName(const QString &wdgtAccessibleName, const QString &wdgtTitle);

    void showWdgtByName(const QString &wdgtAccessibleName, const QString &wdgtTitle, const QIcon &itemIcon);

    void showLastWdgt();

    void updateScrollAreaHeight();

signals:
    void stopExchange();

    void onReloadAllMeters();

    void openEditMacProfileWdgt(bool isEditMode, QLineEdit *le );

private slots:

    void on_tbIfaceSett_clicked();


    void on_trDevOperation_clicked(const QModelIndex &index);

    void addWdgt2devStack(const QString &realPageName, const QString &wdgtTitle, const QIcon &itemIcon);

    void on_pbStop_clicked();


private:
    Ui::StartExchange *ui;
    MatildaConfWidget *currentMatildaWidget();

    MatildaConfWidget *createStartPagePoll(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);

    MatildaConfWidget *createRelayWdgt(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);

    MatildaConfWidget *createMetersDateTime(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);

    MatildaConfWidget *createZbyrIfaceSett(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);

    void showWdgtByNameData(const QString &named);


    QString lastWdgtAccessibleName;
    QString lastWdgtAccessibleNameInProcess;

};

#endif // STARTEXCHANGE_H
