#include "zbyratorucmedium.h"
#include "src/matilda/moji_defy.h"


///[!] matilda-base
#include "matilda-bbb-src/shared/pathsresolver.h"


ZbyratorUcMedium::ZbyratorUcMedium(QObject *parent) : ZbyratorUcMediumBase(PathsResolver::defSqliteMediumLocalServerName(), parent)
{

}
