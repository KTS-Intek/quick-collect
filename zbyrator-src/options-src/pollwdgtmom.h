#ifndef POLLWDGTMOM_H
#define POLLWDGTMOM_H

#include <QObject>
#include "dataconcetrator-pgs/pollwdgt.h"

class PollWdgtMom : public QObject
{
    Q_OBJECT
public:
    explicit PollWdgtMom(PollWdgt *pollWdgt, QObject *parent = nullptr);

    MatildaConfWidget *pollWdgt;
signals:

public slots:
    void onReloadPageSettings();

    void onSavePageSettings();

    void unlockIgnore();

private:
    bool ignoreSave;

};

#endif // POLLWDGTMOM_H
