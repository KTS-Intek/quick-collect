#include "appversion.h"

AppVersion::AppVersion(QObject *parent) : QObject(parent)
{

}

//-------------------------------------------------------------------------------------
QString AppVersion::getVersion()
{

    return tr("<html><head><head/><body><p>Quick Collect</p>"
                   "<p>Version %1</p><p>%2 KTS Intek Ltd, %3</p>"
                   "<p><a href=\"http://kts-intek.com.ua\">"
                   "<span style=\" text-decoration: underline; color:#2980b9;\">"
                   "http://kts-intek.com.ua</span></a></p></body></html>").arg("1.1").arg(QString::fromUtf8("©")).arg("2018") ;//version
}
