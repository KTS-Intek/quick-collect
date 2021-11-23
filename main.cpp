#include "qcmainwindow.h"
#include <QApplication>

#include "guisett-shared-src/appfontloader.h"


int main(int argc, char *argv[])
{
    int r = 0;
    if(r == 0){
        QApplication a(argc, argv);

        const AppFontLoader::LoadFontMemo m = AppFontLoader::loadFontAndSett();
        QcMainWindow w(m.font4log, m.defFontPointSize, qApp->arguments().contains("-vv"));
        w.showSplash();
        r = a.exec();
    }
    return r;
}
