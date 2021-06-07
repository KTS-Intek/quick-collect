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
    explicit QcMainWindow(const QFont &font4log, const int &defFontPointSize, QWidget *parent = nullptr);
    ~QcMainWindow();


signals:
    void reloadSettings2ucEmulator();


    void onRequest2pollThese(QStringList nis, quint8 metertype);
    void onRequest2GetDataThese(QStringList nis, quint8 metertype);
    void tryToActivateThisPage(QString page);

    void command2extensionBBB( quint16 extName, quint16 extCommand, QVariant data);//extId command data, the same as matilda-bbb


public slots:
    void initPage();

    void retranslateWidgets();


    void onActivateThisWdgt(QString tabData, bool andShowIt);





    void continueCreatingObjects();


    void onAppIsKilling();

    void checkDbPageIsReady();

    void activatePageDb();
    void activatePageHome();
    void checkProxySett();


private slots:



    void onStackedWidgetCurrentChanged(int arg1);


    void on_actionOptions_triggered();

    void on_actionAbout_triggered();



    void on_actionExchange_triggered();

    void on_actionProxy_settings_triggered();

    void checkProxySettLater();


private: //functions

    void createOneOfMainWdgt(const QString &tabData, const bool &andActivateIt);

private:
    void createToolBar();

    void createAppOutLog();

    void createZbyrProcManager();

    void createMeterManager();
    void createMeterListManager();

    void createMatildaBBBcover();

    void createOneInstanceChecker();


    MatildaConfWidget *createStartExchangeWdgt(GuiHelper *gHelper, QWidget *parent = 0);

    MatildaConfWidget *createElectricityMeterListWdgt(GuiHelper *gHelper, QWidget *parent = 0);

    MatildaConfWidget *createWaterMeterListWdgt(GuiHelper *gHelper, QWidget *parent = 0);
    MatildaConfWidget *createPulseMeterListWdgt(GuiHelper *gHelper, QWidget *parent = 0);

    MatildaConfWidget *createPageLog(GuiHelper *gHelper, QWidget *parent = 0);

    MatildaConfWidget *createDatabasePage(GuiHelper *gHelper, QWidget *parent = 0);

//    MatildaConfWidget *createMeterLogsPage(GuiHelper *gHelper, QWidget *parent = 0); obsolete

    Ui::QcMainWindow *ui;

    QString lastWdgtAccessibleName;

    ZbyrMeterListMedium *metersListMedium;
};

#endif // QCMAINWINDOW_H
