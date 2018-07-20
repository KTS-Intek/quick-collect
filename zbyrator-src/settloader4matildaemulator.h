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

    static QVariant defaultValue4zbyratorKey(const int &key);

    static QVariantHash defaultPollWdgtSett();
signals:

public slots:
    void updatePollSett();

};

#endif // SETTLOADER4MATILDAEMULATOR_H
