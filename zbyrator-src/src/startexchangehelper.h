#ifndef STARTEXCHANGEHELPER_H
#define STARTEXCHANGEHELPER_H

#include <QObject>
#include <QStringList>

class StartExchangeHelper : public QObject
{
    Q_OBJECT
public:
    explicit StartExchangeHelper(QObject *parent = nullptr);

    static QStringList getChListData(QStringList &listIcos, QStringList &chListNames);

    static QStringList getChList();

    static QString getStts4rez(const quint8 &rez);

signals:

public slots:
};

#endif // STARTEXCHANGEHELPER_H
