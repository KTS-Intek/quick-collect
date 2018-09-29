#ifndef WATERSLEEPSCHEDULESAVER_H
#define WATERSLEEPSCHEDULESAVER_H

#include <QVariantHash>
#include <QMap>

class WaterSleepScheduleSaver
{
public:
    static QVariant getDefaultProfileVar();

    static QVariantHash getDefaultProfile();

    static QVariantHash getDefaultSett();

    static QVariantHash getSavedSett();

    static void addNewProfile(const QString &name, const QVariantHash &profile);

    static void removeOneProfile(const QString &name);

    static QMap<QString,QString> getSavedSettMap();



};

#endif // WATERSLEEPSCHEDULESAVER_H
