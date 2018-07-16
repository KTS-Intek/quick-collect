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


signals:

public slots:
};

#endif // ZBYRTABLEHEADERS_H
