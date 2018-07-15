#ifndef ZBYRATORPROCESSMANAGER_H
#define ZBYRATORPROCESSMANAGER_H

#include <QObject>

class ZbyratorProcessManager : public QObject
{
    Q_OBJECT
public:
    explicit ZbyratorProcessManager(QObject *parent = nullptr);

signals:

public slots:
    void onThreadStarted();



private:


};

#endif // ZBYRATORPROCESSMANAGER_H
