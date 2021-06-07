#include "createtoolbar4quickcollect.h"

CreateToolBar4quickCollect::CreateToolBar4quickCollect(QObject *parent) : CreateToolBar(2, 33, parent)
{

}

QList<TabName2icosPaths> CreateToolBar4quickCollect::getTabs4quickCollect()
{
    QList<TabName2icosPaths> name2icos;

    name2icos.append(TabName2icosPaths(tr("Exchange")   , ":/katynko/svg/go-home.svg"   , "Exchange")   );
    name2icos.append(TabName2icosPaths(tr("Electricity"), ":/katynko/svg4/sc_symbolshapes.lightning.svg"  , "Meters")     );
    name2icos.append(TabName2icosPaths(tr("Water")      , ":/katynko/svg4/lc_fillcolor.svg"  , "WaterMeters")     );
    name2icos.append(TabName2icosPaths(tr("Pulse")      , ":/katynko/svg4/pulse-meter.svg"  , "PulseMeters")     );

    name2icos.append(TabName2icosPaths(tr("Database")   , ":/katynko/svg2/db.svg"       , "Database")   );
    //    name2icos.append(TabName2icosPaths(tr("Logbook")    , ":/katynko/svg2/lc_dbqueryedit.svg", "Logbook")   );

    //    name2icos.append(TabName2icosPaths(tr("KTS Connect"), ":/katynko/svg/document-edit-sign-encrypt.svg", "KTS Connect"));
    name2icos.append(TabName2icosPaths(tr("Log")        , ":/katynko/svg/view-list-text.svg", "Log"));
    return name2icos;
}

QStringList CreateToolBar4quickCollect::getTabsData()
{
    return getTabsDataExt(getTabs4quickCollect());
}

QList<TabName2icosPaths> CreateToolBar4quickCollect::getTabs()
{
    return getTabs4quickCollect();
}
