#ifndef PLATFORMDEPENDCLASS_H
#define PLATFORMDEPENDCLASS_H

#include <QObject>

#include <QtCore>

#define VERSION_4_PC    1

class PlatformDependClass : public QObject
{
    Q_OBJECT
public:
    explicit PlatformDependClass(QObject *parent = 0);

    quint16 getFirstCommand();

    quint16 getLastCommand();

    QHash<QString,quint16> getPlatformCommands();

    QString getMan();

    QStringList getListCommandName();

    QList<quint16> getListCommand();

signals:

public slots:
};

#endif // PLATFORMDEPENDCLASS_H
