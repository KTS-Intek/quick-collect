#ifndef WATERSLEEPSCHEDULESAVER_H
#define WATERSLEEPSCHEDULESAVER_H

#include <QVariantHash>


class WaterSleepScheduleSaver
{
public:
    static QVariant getDefaultProfileVar();

    static QVariantHash getDefaultProfile();

    static QVariantHash getDefaultSett();

    static QVariantHash getSavedSett();

    static void addNewProfile(const QString &name, const QVariantHash &profile);

    static void removeOneProfile(const QString &name);


};

#endif // WATERSLEEPSCHEDULESAVER_H
