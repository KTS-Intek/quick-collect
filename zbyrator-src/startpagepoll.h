#ifndef STARTPAGEPOLL_H
#define STARTPAGEPOLL_H

#include "src/shared/matildaconfwidget.h"
#include "template-pgs/selectdtwdgt.h"

namespace Ui {
class StartPagePoll;
}

class StartPagePoll : public MatildaConfWidget
{
    Q_OBJECT

public:
    explicit StartPagePoll(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);
    ~StartPagePoll();

    void setupCbxModel2regExp(QComboBox *cbx, const QVariantHash &hash);

    QVariant getPageSett4read(bool &ok, QString &mess);

signals:
    void onReloadAllMeters();

    void appendData2model(QString wdgtAccsbltName, QVariantHash h);

    void killTabByName(QString wdgtAccsbltName);

    void command4dev(quint16 command, QString args);//pollCode args

public slots:
    void initPage();

    void onUpdatedSavedList(int activeMetersSize, int switchedOffMetersSize);

    void disconnectMeFromAppendData();

    void setPbReadEnbld();

private slots:
    void on_pbReadDb_clicked();


    void on_rbSavedList_clicked();

    void on_rbOneMeter_clicked();

    void changeRule2le();

    void updatePhValSett();

    void on_swMeterMode_currentChanged(int arg1);

    void onLvMeterDataProfile_activated(const QModelIndex &index);

private:
    Ui::StartPagePoll *ui;

    bool startPollOneMeterMode(const quint8 &pollCode, QString &mess);


    SelectDtWdgt *dtFromToWdgt;
    QString lastWdgtAccssbltName;
    QStandardItemModel *modelProfile4DB;

    bool multipleML;


};

#endif // STARTPAGEPOLL_H
