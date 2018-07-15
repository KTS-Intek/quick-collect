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


    void onConfigChanged(quint16 command, QVariant data);

    void command4dev(quint16 command, QString str);


    //meterlistwdgt
    void onAllMeters(UniversalMeterSettList allMeters);
    void onReloadAllMeters();

public slots:
    void initializeZbyrator();

    void onLangSelected(QString lang);

    void loadFontSize();

    void onActivateThisWdgt(QString tabData);

    void addWdgt2stackWdgt(QWidget *w, int wdgtTag, bool oneShot, QString actTxt, QString actIco);

private slots:



    void onStackedWidgetCurrentChanged(int arg1);

protected:
    void changeEvent(QEvent *event);

private: //functions
    void loadLanguage(const QString& rLanguage);

    void createOneOfMainWdgt(const QString &tabData);

private:
    void createToolBar();

    void createZbyrProcManager();

    void createMeterManager();

    MatildaConfWidget *createMeterListWdgt(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);

    Ui::QcMainWindow *ui;
    QTranslator translator;

    QString lastWdgtAccessibleName;
};

#endif // QCMAINWINDOW_H
