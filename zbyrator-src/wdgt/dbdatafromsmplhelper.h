#ifndef DBDATAFROMSMPLHELPER_H
#define DBDATAFROMSMPLHELPER_H

#include <QObject>
#include <QStringList>


struct DbHeaderSett
{
    QStringList listCol;
    QStringList listData;

    DbHeaderSett() {}
    DbHeaderSett(const QStringList &listCol, const QStringList &listData) : listCol(listCol), listData(listData) {}
};

class DbDataFromSmplHelper : public QObject
{
    Q_OBJECT
public:
    explicit DbDataFromSmplHelper(QObject *parent = nullptr);

    static DbHeaderSett showHideCb4column4Db(const int &mode, const QStringList &lHeaderData);

    static QStringList checkShowColumn4Db(const int &lastDbFilterMode, const QStringList &lastColumnList4DB, QStringList listEnrg);

    static int getLastDbModeFromPollCode(const quint8 &pollCode);

    static int getLastDbModeFromPollCode(const quint8 &pollCode, const bool &isZbyrator, QVariantHash &hash);

    static bool getAllowDate2utc(const quint8 &code);

signals:

public slots:
};

#endif // DBDATAFROMSMPLHELPER_H
