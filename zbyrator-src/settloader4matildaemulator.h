#ifndef SETTLOADER4MATILDAEMULATOR_H
#define SETTLOADER4MATILDAEMULATOR_H

#include <QObject>
#include <QVariantHash>
#include <QStringList>

class SettLoader4matildaEmulator : public QObject
{
    Q_OBJECT
public:
    explicit SettLoader4matildaEmulator(QObject *parent = nullptr);

    QHash<int, QVariant> hashSett;

    QVariant loadOneSett(const int &key);

    bool saveOneSett(const int key, const QVariant data2save);

signals:

public slots:
};

#endif // SETTLOADER4MATILDAEMULATOR_H
