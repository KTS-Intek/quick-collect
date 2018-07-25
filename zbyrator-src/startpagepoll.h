#ifndef STARTPAGEPOLL_H
#define STARTPAGEPOLL_H

#include "src/shared/matildaconfwidget.h"
#include "template-pgs/selectdtwdgt.h"
//#include "src/zbyrator-v2/zbyratordatatypehelper.h"
#include "zbyrator-src/zbyrmeterlistmedium.h"

namespace Ui {
class StartPagePoll;
}

class StartPagePoll : public MatildaConfWidget
{
    Q_OBJECT

public:
    explicit StartPagePoll(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);
    ~StartPagePoll();
    ZbyrMeterListMedium *metersListMedium;

    void setupCbxModel2regExp(QComboBox *cbx, const QVariantHash &hash);

    QVariant getPageSett4read(bool &ok, QString &mess);

signals:
    void onReloadAllMeters();

    void appendData2model(QString wdgtAccsbltName, QVariantHash h);

    void killTabByName(QString wdgtAccsbltName);

    void command4dev(quint16 command, QString args);//pollCode args
    void command4dev(quint16 command, QVariantMap mapArgs);//pollCode args

    void onPollStarted(quint8 pollCode, QStringList listEnrg, QString dateMask, int dotPos, bool allowDate2utc);

    void killSelectMeters4poll();
//    void onAllMeters(UniversalMeterSettList allMeters);

    void setIgnoreCycles(bool ignoreCycles);
public slots:
    void initPage();

    void setPageSett(const QVariantHash &h);


    void onUpdatedSavedList(int activeMetersSize, int switchedOffMetersSize, int meterElectricityActive, int metersWaterActive);

    void disconnectMeFromAppendData();

    void setPbReadEnbld();

    void command4devSlot(quint16 command, QVariantMap map);//pollCode args


private slots:
    void on_pbReadDb_clicked();


    void on_rbSavedList_clicked();

    void on_rbOneMeter_clicked();

    void changeRule2le();

    void updatePhValSett();

    void on_swMeterMode_currentChanged(int arg1);

    void onLvMeterDataProfile_activated(const QModelIndex &index);

    void on_tabWidget_tabCloseRequested(int index);

private:
    Ui::StartPagePoll *ui;

    bool startPollOneMeterMode(const quint8 &pollCode, QString &mess);

    bool startPollAllMetersMode(const quint8 &pollCode, QString &mess);


    void createTab(const quint8 &code);

    QStringList getEnrgList4code(const quint8 &code);

    SelectDtWdgt *dtFromToWdgt;
    QString lastWdgtAccssbltName;
    QStandardItemModel *modelProfile4DB;

    bool multipleML;

    quint8 lastPollCode;

};

#endif // STARTPAGEPOLL_H
