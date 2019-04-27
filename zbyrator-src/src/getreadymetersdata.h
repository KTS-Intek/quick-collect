#ifndef GETREADYMETERSDATA_H
#define GETREADYMETERSDATA_H

#include <QObject>
#include <QStringList>
#include <QDateTime>

///[!] zbyrator-settings
#include "src/meter/zbyratorfilesetthelper.h"

///[!] zbyrator-base
#include "src/zbyrator-v2/meterschedulerhelper.h"
#include "src/zbyrator-v2/dataprocessinghelper.h"

///[!] task-base
#include "src/zbyrator-v2/meterschedulertypes.h"

#include "classmanagertypes.h"

class GetReadyMetersData : public QObject
{
    Q_OBJECT
public:
    explicit GetReadyMetersData(QObject *parent = nullptr);

    static quint8 meterTypeFromPollCode(const quint8 &code);

signals:
    void appendMess(QString mess);

    void allMeters2selectWdgt(MyListStringList listRows, QVariantMap col2data, QStringList headerH, QStringList header, bool hasHeader);

    void onProcessingEnds(QStringList listMissingData);

    void closeDbConnection();

public slots:
    void setPollSett(QDateTime dtFrom, QDateTime dtTo, quint8 pollCode, quint8 meterType);

    void setMetersList(UniversalMeterSettList list);


    void onThreadStarted();

    void stopAllDirect();

private:
    void doCalculation(const OneProfileSett &oneprofile);

    bool checkDbIsOpen();


    void addAllMeters2table();


    DataProcessingHelper *dbProcessingHelper;

    bool stopAll;

    UniversalMeterSettList meters;

    struct LastPollSett{
        QDateTime dtFrom;
        QDateTime dtTo;
        quint8 pollCode;
        quint8 meterType;
    } lPollSett;

};

#endif // GETREADYMETERSDATA_H
