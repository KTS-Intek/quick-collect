#include "createtoolbar.h"
#include <QTimer>
#include <QAction>

CreateToolBar::CreateToolBar(QObject *parent) : QObject(parent)
{
    tb = 0;
    activePageCounter = 0;
}

QList<TabName2icosPaths> CreateToolBar::getTabs()
{

    QList<TabName2icosPaths> name2icos;

    name2icos.append(TabName2icosPaths(tr("Exchange")   , ":/katynko/svg/go-home.svg"   , "Exchange")   );
    name2icos.append(TabName2icosPaths(tr("Electricity"), ":/katynko/svg4/sc_symbolshapes.lightning.svg"  , "Meters")     );
    name2icos.append(TabName2icosPaths(tr("Water")      , ":/katynko/svg4/lc_fillcolor.svg"  , "WaterMeters")     );

    name2icos.append(TabName2icosPaths(tr("Database")   , ":/katynko/svg2/db.svg"       , "Database")   );
//    name2icos.append(TabName2icosPaths(tr("Logbook")    , ":/katynko/svg2/lc_dbqueryedit.svg", "Logbook")   );

//    name2icos.append(TabName2icosPaths(tr("KTS Connect"), ":/katynko/svg/document-edit-sign-encrypt.svg", "KTS Connect"));
    name2icos.append(TabName2icosPaths(tr("Log")        , ":/katynko/svg/view-list-text.svg", "Log"));
    return name2icos;
}

QStringList CreateToolBar::getTabsData()
{
    const QList<TabName2icosPaths> name2icos = getTabs();
    QStringList l;
    for(int i = 0, imax = name2icos.size(); i < imax; i++)
        l.append(name2icos.at(i).tabData);
    return l;
}

void CreateToolBar::createToolBarItems(QToolBar *tb)
{
    this->tb = tb;
    tb->setContextMenuPolicy(Qt::CustomContextMenu);
    emit killAllActions();
    activePageCounter = 0;
    QTimer::singleShot(333, this, SLOT(addFirstItem()));

}

void CreateToolBar::addFirstItem()
{
    name2icos = getTabs();
    firstPageTabData = name2icos.first().tabData;
    addItem2tb(name2icos.takeFirst(), true);
    QTimer::singleShot(66, this, SLOT(addNextActiveItem()));
}

void CreateToolBar::addNextActiveItem()
{
    if(activePageCounter > 2){


        addNextItem();
        QTimer::singleShot(111, this, SLOT(showMainPage()));
        return;
    }
    if(name2icos.isEmpty())
        return;

    addItem2tb(name2icos.takeFirst(), true);
    QTimer::singleShot(33, this, SLOT(addNextActiveItem()));

}

void CreateToolBar::addNextItem()
{
    if(name2icos.isEmpty())
        return;

    addItem2tb(name2icos.takeFirst(), false);
    QTimer::singleShot(33, this, SLOT(addNextItem()));
}

void CreateToolBar::onActImitatorClck()
{
    emit setCheckedAct(false);
    QAction *a = qobject_cast<QAction*>(sender());
    if(a && !a->data().toString().isEmpty()){
        emit onActivateThisWdgt(a->data().toString());
        a->setChecked(true);
    }


}

void CreateToolBar::showMainPage()
{
    if(!firstPageTabData.isEmpty())
        emit animateClckMain();
//        emit onActivateThisWdgt(firstPageTabData);
    firstPageTabData.clear();

}


void CreateToolBar::addItem2tb(const TabName2icosPaths &item, const bool &activate)
{
    if(tb){
        QAction *a = new QAction(QIcon(item.path), item.tabName, parent());
        a->setData(item.tabData);
        a->setCheckable(true);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(onActImitatorClck()) );
        connect(this, SIGNAL(killAllActions()), a, SLOT(deleteLater()) );
        connect(this, SIGNAL(setCheckedAct(bool)), a, SLOT(setChecked(bool)) );

        tb->addAction(a);
        if(activate){
            if(activePageCounter == 0)
                connect(this, SIGNAL(animateClckMain()), a, SLOT(trigger()));

            activePageCounter++;
            QTimer::singleShot(10, a, SLOT(trigger()));
        }
    }
}
