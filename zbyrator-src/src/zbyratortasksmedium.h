#ifndef ZBYRATORTASKSMEDIUM_H
#define ZBYRATORTASKSMEDIUM_H

#include <QObject>



#include "ucdevicetreetypes.h"

class ZbyratorTasksMedium : public QObject
{
    Q_OBJECT
public:
    explicit ZbyratorTasksMedium(QObject *parent = nullptr);



signals:

    void onTaskTableChanged();


    void setUCTaskTableStateMeter(UCTaskTableState info);


    void startTmrSendTaskTable(int msec);




public slots:
    void onThreadStarted();


    void sendTaskTableLater();


    void onTmrSendTaskTableLater();



    void try2sendTaskTable();




};

#endif // ZBYRATORTASKSMEDIUM_H
