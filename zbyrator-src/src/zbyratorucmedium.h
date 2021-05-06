#ifndef ZBYRATORUCMEDIUM_H
#define ZBYRATORUCMEDIUM_H

///[!] parametryzator-base
#include "gui-db-shared-src/zbyratorucmediumbase.h"


//it is only for compatible with quick poll,

class ZbyratorUcMedium : public ZbyratorUcMediumBase
{
    Q_OBJECT
public:
    explicit ZbyratorUcMedium(QObject *parent = nullptr);


};

#endif // ZBYRATORUCMEDIUM_H
