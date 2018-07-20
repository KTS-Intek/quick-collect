#ifndef APPVERSION_H
#define APPVERSION_H

#include <QObject>

class AppVersion : public QObject
{
    Q_OBJECT
public:
    explicit AppVersion(QObject *parent = nullptr);

    static QString getVersion();

signals:

public slots:
};

#endif // APPVERSION_H
