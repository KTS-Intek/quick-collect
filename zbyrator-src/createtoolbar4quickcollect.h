#ifndef CREATETOOLBAR4QUICKCOLLECT_H
#define CREATETOOLBAR4QUICKCOLLECT_H

#include "zbyrator-src/createtoolbar.h"


class CreateToolBar4quickCollect : public CreateToolBar
{
    Q_OBJECT
public:
    explicit CreateToolBar4quickCollect(QObject *parent = nullptr);

    static QList<TabName2icosPaths> getTabs4quickCollect();

    static QStringList getTabsData();

    QList<TabName2icosPaths> getTabs();

signals:

public slots:
};

#endif // CREATETOOLBAR4QUICKCOLLECT_H
