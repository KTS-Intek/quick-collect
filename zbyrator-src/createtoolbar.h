#ifndef CREATETOOLBAR_H
#define CREATETOOLBAR_H

#include <QObject>
#include <QToolBar>
#include <QHash>
#include <QString>

struct TabName2icosPaths
{
    QString tabName;
    QString path;
    QString tabData;
    TabName2icosPaths() {}
    TabName2icosPaths(const QString &tabName, const QString &path, const QString &tabData) : tabName(tabName), path(path), tabData(tabData) {}
};


class CreateToolBar : public QObject
{
    Q_OBJECT
public:
    explicit CreateToolBar(QObject *parent = nullptr);

    static QList<TabName2icosPaths> getTabs();

    static QStringList getTabsData();

signals:
    void killAllActions();

    void onActivateThisWdgt(QString tabData);

    void setCheckedAct(bool checked);


    void animateClckMain();
public slots:
    void createToolBarItems(QToolBar *tb);

    void addFirstItem();

    void addNextActiveItem();

    void addNextItem();

    void onActImitatorClck();

    void showMainPage();


private:
    void addItem2tb(const TabName2icosPaths &item, const bool &activate);

    QToolBar *tb;

    int activePageCounter;
    QList<TabName2icosPaths> name2icos;
    QString firstPageTabData;

};

#endif // CREATETOOLBAR_H
