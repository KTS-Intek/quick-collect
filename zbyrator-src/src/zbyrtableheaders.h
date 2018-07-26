#ifndef ZBYRTABLEHEADERS_H
#define ZBYRTABLEHEADERS_H

#include <QObject>
#include <QStringList>


class ZbyrTableHeaders : public QObject
{
    Q_OBJECT
public:
    explicit ZbyrTableHeaders(QObject *parent = nullptr);

    static QStringList getRelayPageHeader();

    static QStringList getMeterDateTimePageHeader();

    static QStringList getWaterMeterSchedulerPageHeader();

signals:

public slots:
};

#endif // ZBYRTABLEHEADERS_H
