#include "qcmainwindow.h"
#include "ui_qcmainwindow.h"
#include "src/matilda/settloader.h"
#include "main-pgs/langdialog.h"
#include <QDebug>
#include <QTimer>
#include <QLocale>
#include <QDesktopWidget>
#include <QStyleFactory>
#include "zbyrator-src/createtoolbar.h"
#include "src/shared/stackwidgethelper.h"

#include "dataconcetrator-pgs/meterlistwdgt.h"
#include "src/matilda/moji_defy.h"
#include "dataconcetrator-pgs/databasewdgt.h"
#include "dataconcetrator-pgs/editenergywidget.h"
#include "dataconcetrator-pgs/meterjournalform.h"
#include "template-pgs/smplptewdgt.h"
#include "zbyrator-src/ktsconnectwdgt.h"
#include "zbyrator-src/startexchange.h"


QcMainWindow::QcMainWindow(const QFont &font4log, const int &defFontPointSize,  QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QcMainWindow)
{
    ui->setupUi(this);

    guiSett = new GuiSett4all(this);
    guiSett->font4log = font4log;
    guiSett->defFontPointSize = defFontPointSize;

    loadMainSett();


    GuiSett4all::setKtsIntekDefautStyle();

    QTimer::singleShot(999, this, SLOT(initializeZbyrator())) ;
}

QcMainWindow::~QcMainWindow()
{
    delete ui;
}

void QcMainWindow::loadMainSett()
{

    QRect rect = SettLoader::loadSett(SETT_MAIN_GEOMETRY).toRect();
    if(!rect.isNull()){
        qint32 desktopW = qApp->desktop()->width();
        qint32 desktopH = qApp->desktop()->height();

        if(rect.x()< 10)
            rect.setX(10);
        if(rect.y()<30)
            rect.setY(31);
        if(rect.x()>(desktopW)  || rect.y()>(desktopH)) {
            int lastW = rect.width();
            int lastH = rect.height();
            rect.setX(10);
            rect.setY(31);
            rect.setWidth(lastW);
            rect.setHeight(lastH);
        }
        setGeometry(rect);
    }


    if(SettLoader::loadSett(SETT_MAIN_LANG_SELECTED).toBool()){
         loadLanguage(SettLoader::loadSett(SETT_MAIN_CURRLANG).toString());

    }else{
        LangDialog *d = new LangDialog(this);
        connect(d, SIGNAL(onLangSelected(QString)), this, SLOT(onLangSelected(QString)) );
        if(d->exec() == QDialog::Rejected)
            QTimer::singleShot(11, this, SLOT(loadFontSize()) );
        d->deleteLater();
    }
}

void QcMainWindow::initializeZbyrator()
{
    qRegisterMetaType<QHash<QString,QString> >("QHash<QString,QString>");
//    qRegisterMetaType<MyListStringList>("MyListStringList");

    guiHelper = new GuiHelper(this);

    guiHelper->initObj();
    guiHelper->guiSett = guiSett;
    guiHelper->lDevInfo = new LastDevInfo(this);
    guiHelper->lDevInfo->matildaDev.protocolVersion = MATILDA_PROTOCOL_VERSION;

    connect(ui->stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(onStackedWidgetCurrentChanged(int)) );
    connect(guiHelper, SIGNAL(addWdgt2stackWdgt(QWidget*,int,bool,QString,QString)), this ,SLOT(addWdgt2stackWdgt(QWidget*,int,bool,QString,QString)) );

//    connect(guiHelper, SIGNAL(addWdgt2stackWdgt(QWidget*,int)   ), this, SLOT(addWdgt2stackWdgt(QWidget*,int))  );
//    connect(guiHelper, SIGNAL(loadDevList()                     ), this, SLOT(loadDevList())                    );

    connect(guiSett, SIGNAL(changeLanguate(QString)), this, SLOT(onLangSelected(QString)) );
    connect(guiSett, SIGNAL(updateStyle(QString))   , this, SLOT(setStyleSheet(QString))  );

    guiHelper->cacheHelper = new MatildaCacheHelper(this);

    StackWidgetHelper *swh = new StackWidgetHelper(this);
    swh->setStackWdgt(ui->stackedWidget);
    connect(this, SIGNAL(addWdgt2history()), swh, SLOT(onAddWdgt2history()) );
    connect(ui->stackedWidget, SIGNAL(destroyed(QObject*)), swh, SLOT(onStackKickedOff()));

    CreateToolBar *c = new CreateToolBar(this);
    connect(c, SIGNAL(onActivateThisWdgt(QString)), this, SLOT(onActivateThisWdgt(QString)) );
    c->createToolBarItems(ui->mainToolBar);




}

void QcMainWindow::onLangSelected(QString lang)
{
    if(lang.isEmpty())
        return;

    SettLoader::saveSett(SETT_MAIN_LANG_SELECTED, true);
    SettLoader::saveSett(SETT_MAIN_CURRLANG, lang);
    guiSett->currLang.clear();
    loadLanguage(lang);
    //    loadMainSett();

    if(guiSett->verboseMode)
        qDebug() << "onLang selected" << lang;
}

void QcMainWindow::loadFontSize()
{
    int fontSize = SettLoader::loadSett(SETT_OPTIONS_APPFONT, guiSett->defFontPointSize).toInt();
    if(fontSize < 6 || fontSize > 300)
        fontSize = guiSett->defFontPointSize;
    if(fontSize > 6 && fontSize < 300)
        setStyleSheet(QString("font-size:%1pt;").arg(fontSize));
}

void QcMainWindow::onActivateThisWdgt(QString tabData)
{
    //stackContainsThisWdgt(QStackedWidget *stackedWidget, const QString &wdgtName, const bool andChange, const int indxFrom)
    if(!GuiHelper::stackContainsThisWdgt(ui->stackedWidget, tabData, true))
        createOneOfMainWdgt(tabData);

    emit addWdgt2history();
}

void QcMainWindow::addWdgt2stackWdgt(QWidget *w, int wdgtTag, bool oneShot, QString actTxt, QString actIco)
{
    emit killOldWdgt();

    QString wdgtName = QString("Wdgt4stack_%1").arg(wdgtTag);
    for(int i = 1, iMax = ui->stackedWidget->count(); i < iMax; i++){
        if(ui->stackedWidget->widget(i)->accessibleName() == wdgtName){
            ui->stackedWidget->widget(i)->deleteLater();
            break;
        }
    }

    w->setAccessibleName(wdgtName);

//    QAction *a = new QAction(w);


//    switch(oneShot){
//    case WDGT_TYPE_EDITENERGYWIDGET     : a->setText(tr("Edit Energy"))         ; a->setIcon(QIcon(":/katynko/svg/document-edit.svg")); break;
//    case WDGT_TYPE_RENAMEOBJECTWIDGET   : a->setText(tr("Rename Object"))       ; a->setIcon(QIcon(":/katynko/svg/address-book-new.svg")); break;
//    case WDGT_TYPE_SCANIPWIDGET         : a->setText(tr("Scan"))                ; a->setIcon(QIcon(":/katynko/svg/irc-voice.svg")); break;
//    default: oneShot = true; break;
//    }

    if(oneShot)
        connect(this, SIGNAL(killOldWdgt()), w, SLOT(deleteLater()) );//залишу тільки для окремих випадків


    if(!actTxt.isEmpty()){
        QAction *a = new QAction(QIcon(actIco), actTxt, w);
        a->setData(wdgtTag);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(onActImitatorClck()) );

        QAction *s = ui->mainToolBar->addSeparator();
        connect(a, SIGNAL(destroyed(QObject*)), s, SLOT(deleteLater()) );

        ui->mainToolBar->addAction(a);
    }

    if(wdgtTag == WDGT_TYPE_DEVICES){
        ui->stackedWidget->addWidget(w);
        ui->stackedWidget->setCurrentWidget(w);
    }else{
        QScrollArea *sa = StackWidgetHelper::addWdgtWithScrollArea(this, w, wdgtName);
        ui->stackedWidget->addWidget(sa);
        ui->stackedWidget->setCurrentWidget(sa);
    }
    emit addWdgt2history();
}

void QcMainWindow::changeEvent(QEvent *event)
{
    if(event) {
        switch(event->type()) {
        // this event is send if a translator is loaded
        case QEvent::LanguageChange: {
            ui->retranslateUi(this);
            break; }
            // this event is send, if the system, language changes
        case QEvent::LocaleChange: {
            QString locale = QLocale::system().name();
            locale.truncate(locale.lastIndexOf('_'));
            loadLanguage(locale);
            break;}
        default: break;
        }
    }
    QWidget::changeEvent(event);
}

void QcMainWindow::loadLanguage(const QString &rLanguage)
{
    if(guiSett->currLang != rLanguage) {
        guiSett->currLang = rLanguage;
        QString langPath = QApplication::applicationDirPath();
        QLocale locale = QLocale(rLanguage);
        QLocale::setDefault(locale);

        qApp->removeTranslator(&translator);

        if(translator.load(QString("%1/lang/%2").arg(langPath).arg( QString("lang_%1.qm").arg(rLanguage)))){
            qApp->installTranslator(&translator);
        }else{
            if(guiSett->verboseMode)
                qDebug() << "errr load " << QString("%1/lang/%2").arg(langPath).arg( QString("lang_%1.qm").arg(rLanguage));
        }
        QTimer::singleShot(32, this, SLOT(loadFontSize()) );

    }
}

void QcMainWindow::createOneOfMainWdgt(const QString &tabData)
{
    qDebug() << "create " << tabData;

    LastDevInfo *lDevInfo = guiHelper->lDevInfo;
    GuiHelper *gHelper = guiHelper;
    GuiSett4all *gSett4all = guiSett;

    const int row = CreateToolBar::getTabsData().indexOf(tabData);
    MatildaConfWidget *w = 0;

    switch(row){
    case 0: w = new StartExchange(lDevInfo, gHelper, gSett4all, this); break;
    case 1: w = new MeterListWdgt(lDevInfo, gHelper, gSett4all, this); break;

    case 2: w = new DataBaseWdgt( lDevInfo, gHelper, gSett4all, this) ; break;  //    l.append( QString("Database") );
    case 3: w = new MeterJournalForm(lDevInfo, gHelper, gSett4all, this) ; break; //    l.append( QString("Meter logs") );
    case 4: w = new KtsConnectWdgt(lDevInfo, gHelper, gSett4all, this); break;
    case 5: w = new SmplPteWdgt(tr("Log") , true, false,   lDevInfo, gHelper, gSett4all, false, this)  ; break;   //    l.append( QString("State")                   );

    }

    if(w){
        try{
            w->objectName();
        }catch(...){
            w = 0;
            return;
        }
    }

    if(w){
        const TabName2icosPaths ttn = CreateToolBar::getTabs().at(row);

        w->setWindowTitle(ttn.tabName);
        w->setWindowIcon(QIcon(ttn.path));


//        connect(this, SIGNAL(killMyChild()), w, SLOT(deleteLater()) );
//        connect(this, SIGNAL(onOperationNError(int)), w, SLOT(onOperationNError(int)) );
//        w->setRwCommand(MatildaDeviceTree::getPageCanRead().at(row), MatildaDeviceTree::getPageCanWrite().at(row));

        w->setRwCommand(1, 0xFFFE);
        w->setHasDataFromRemoteDevice();
        w->setupGlobalLblMessage(ui->lblPageMess);
        QWidget *sa = StackWidgetHelper::addWdgtWithScrollArea(this, w, tabData);
        ui->stackedWidget->addWidget(sa);
        ui->stackedWidget->setCurrentWidget(sa);
    }
}

void QcMainWindow::onStackedWidgetCurrentChanged(int arg1)
{
    if(arg1 < 0 || !ui->stackedWidget->currentWidget())
        return;
    const QString wdgtAccessibleName = ui->stackedWidget->currentWidget()->accessibleName();
    if(wdgtAccessibleName != lastWdgtAccessibleName){
        guiHelper->closeYourPopupsSlot();
        lastWdgtAccessibleName = wdgtAccessibleName;
    }
}
