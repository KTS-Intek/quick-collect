#ifndef QCMAINWINDOW_H
#define QCMAINWINDOW_H

#include <QMainWindow>
#include <QFont>
#include <QTranslator>
#include <QTabBar>
#include "src/shared/guisett4all.h"
#include "src/matilda/guihelper.h"
#include <QLabel>
#include "src/shared/referencewidgetclass.h"
#include "src/zbyrator-v2/zbyratordatatypehelper.h"

#include "zbyrator-src/zbyrmeterlistmedium.h"



namespace Ui {
class QcMainWindow;
}

class QcMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QcMainWindow(const QFont &font4log, const int &defFontPointSize, QWidget *parent = 0);
    ~QcMainWindow();


    GuiSett4all *guiSett;
    GuiHelper *guiHelper;

    void loadMainSett();

signals:
    void addWdgt2history();
    void killOldWdgt();



    void setThisObjProfileName(QString profileName);

    void appendShowMess(QString m);

    void appendShowMessPlain(QString m);

public slots:
    void initializeZbyrator();

    void onLangSelected(QString lang);

    void loadFontSize();

    void onActivateThisWdgt(QString tabData);

    void addWdgt2stackWdgt(QWidget *w, int wdgtTag, bool oneShot, QString actTxt, QString actIco);

    void openEditMacProfileWdgt(bool isEditMode, QLineEdit *le );


    void showMess(QString mess);

    void showMessSmpl(QString mess);

    void showMessCritical(QString mess);

    void showMess(QString mess, int messType, const QVariant customData = QVariant());

    void onScanClicked(const int &mode, IfaceSettLoader *connWdgt);



    void onActImitatorClck();

private slots:



    void onStackedWidgetCurrentChanged(int arg1);

    void on_actionThe_log_of_the_application_triggered(bool checked);

    void on_actionOptions_triggered();

    void on_actionAbout_triggered();

protected:
    void changeEvent(QEvent *event);

private: //functions
    void loadLanguage(const QString& rLanguage);

    void createOneOfMainWdgt(const QString &tabData);

private:
    void createToolBar();

    void createAppOutLog();

    void createZbyrProcManager();

    void createMeterManager();

    MatildaConfWidget *createStartExchangeWdgt(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);

    MatildaConfWidget *createMeterListWdgt(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);

    MatildaConfWidget *createPageLog(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);

    MatildaConfWidget *createDatabasePage(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);

    MatildaConfWidget *createMeterLogsPage(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);

    Ui::QcMainWindow *ui;
    QTranslator translator;

    QString lastWdgtAccessibleName;

    ZbyrMeterListMedium *metersListMedium;
};

#endif // QCMAINWINDOW_H
