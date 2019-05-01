#ifndef QCMAINWINDOW_H
#define QCMAINWINDOW_H

///[!] widgets-shared
#include "gui-src/wdgt/mainifacemedium.h"
#include "gui-src/referencewidgetclass.h"


///[!] quick-collect
#include "zbyrator-src/zbyrmeterlistmedium.h"


///[!] task-types
#include "src/zbyrator-v2/zbyratordatatypehelper.h"


#include <QTabBar>
#include <QLabel>


namespace Ui {
class QcMainWindow;
}

class QcMainWindow : public MainIfaceMedium
{
    Q_OBJECT

public:
    explicit QcMainWindow(const QFont &font4log, const int &defFontPointSize, QWidget *parent = 0);
    ~QcMainWindow();



signals:
    void reloadSettings2ucEmulator();



public slots:
    void initPage();

    void retranslateWidgets();


    void onActivateThisWdgt(QString tabData);




private slots:



    void onStackedWidgetCurrentChanged(int arg1);


    void on_actionOptions_triggered();

    void on_actionAbout_triggered();



private: //functions

    void createOneOfMainWdgt(const QString &tabData);

private:
    void createToolBar();

    void createAppOutLog();

    void createZbyrProcManager();

    void createMeterManager();

    void createMatildaBBBcover();

    MatildaConfWidget *createStartExchangeWdgt(GuiHelper *gHelper, QWidget *parent = 0);

    MatildaConfWidget *createElectricityMeterListWdgt(GuiHelper *gHelper, QWidget *parent = 0);

    MatildaConfWidget *createWaterMeterListWdgt(GuiHelper *gHelper, QWidget *parent = 0);

    MatildaConfWidget *createPageLog(GuiHelper *gHelper, QWidget *parent = 0);

    MatildaConfWidget *createDatabasePage(GuiHelper *gHelper, QWidget *parent = 0);

//    MatildaConfWidget *createMeterLogsPage(GuiHelper *gHelper, QWidget *parent = 0); obsolete

    Ui::QcMainWindow *ui;

    QString lastWdgtAccessibleName;

    ZbyrMeterListMedium *metersListMedium;
};

#endif // QCMAINWINDOW_H
