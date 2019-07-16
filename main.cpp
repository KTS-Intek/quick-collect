#include "qcmainwindow.h"
#include <QApplication>

#include "guisett-shared-src/appfontloader.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    const AppFontLoader::LoadFontMemo m = AppFontLoader::loadFontAndSett();
    QcMainWindow w(m.font4log, m.defFontPointSize);
    w.showSplash();
    return a.exec();
}
