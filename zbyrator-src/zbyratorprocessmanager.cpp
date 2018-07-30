#include "zbyratorprocessmanager.h"
#include <QApplication>
#include <QFileInfo>

#include "sqlite-medium-src/sqlitemediumlocalserver.h"

ZbyratorProcessManager::ZbyratorProcessManager(QObject *parent) : QObject(parent)
{

}

void ZbyratorProcessManager::onThreadStarted()
{
//    const QString appDirPath = qApp->applicationDirPath();
//    const QString appName = "sqlite-medium";
//    const QStringList args;
//    const bool verboseMode = false;
//    const int procid = 7; //PROC_ID_SQLITEMDM
//    const bool disableApp = false;

//    QFileInfo fi(QString("%1/%2").arg(appDirPath).arg(appName));

//    MatildaChildProcess *procSqliteMedium = new MatildaChildProcess(this);
//    procSqliteMedium->initProces(verboseMode, false,  !fi.exists(), procid, disableApp);
//    procSqliteMedium->startAppSett(appDirPath, appName, args);



    SQLiteMediumLocalServer *m = new SQLiteMediumLocalServer(this);

    QTimer::singleShot(3111, m ,SLOT(initObj()) );
//    m->initObj();
}
