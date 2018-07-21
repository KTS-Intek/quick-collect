#ifndef ZBYRATORTASKSMEDIUM_H
#define ZBYRATORTASKSMEDIUM_H

#include <QObject>
#include "src/zbyrator-v2/zbyratordatatypehelper.h"
#include "src/matilda/classmanagertypes.h"


class ZbyratorTasksMedium : public QObject
{
    Q_OBJECT
public:
    explicit ZbyratorTasksMedium(QObject *parent = nullptr);

    ZbyratorTaskTable getTaskTable();


signals:
    void setZbyratorTasksPageSett(MyListStringList listRows, QVariantMap col2data, QStringList headerH, QStringList header, bool hasHeader);


public slots:
    void onThreadStarted();

    void onTaskTableChanged();

    void onPageReady();

    void onZbyratorTasksPageSett(QVariantHash h);


private:
    bool sendUpdate;
    bool canSendUpdate, pageIsReady;
    QString dateMask;

};

#endif // ZBYRATORTASKSMEDIUM_H