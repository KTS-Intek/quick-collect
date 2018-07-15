#ifndef ZBYRMETERLISTMEDIUM_H
#define ZBYRMETERLISTMEDIUM_H

#include <QObject>
#include "src/zbyrator-v2/zbyratordatatypehelper.h"

class ZbyrMeterListMedium : public QObject
{
    Q_OBJECT
public:
    explicit ZbyrMeterListMedium(QObject *parent = nullptr);

signals:
    void setPageSett(QVariantHash h);

    void realoadMeters2meterManager();//if changed NI, model, SN, removed meter, added meter,


    void startTmrSaveLater();

    void onConfigChanged(quint16 command, QVariant data);

public slots:
    void onAllMeters(UniversalMeterSettList allMeters);

    void meterModelChanged(QVariantList meters);

    void onSaveLater();

private:
    QVariantList lastMeterList;
};

#endif // ZBYRMETERLISTMEDIUM_H
