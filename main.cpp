#include "qcmainwindow.h"
#include <QApplication>

#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    const int defFontPointSize = qApp->font().pointSize();
    int fontId = QFontDatabase::addApplicationFont(":/font/Noto/NotoSans-Regular.ttf");
    if(fontId >= 0 && !QFontDatabase::applicationFontFamilies(fontId).isEmpty()){
        QFont font(QFontDatabase::applicationFontFamilies(fontId).first());
        font.setStyleStrategy(QFont::PreferAntialias);
//        font.setPointSize(fontSize);
        qApp->setFont(font);
        QApplication::setFont(font);
    }
    QFont font4log = qApp->font();

    fontId = QFontDatabase::addApplicationFont(":/font/UbuntuMono-R.ttf");
    if(fontId >= 0 && !QFontDatabase::applicationFontFamilies(fontId).isEmpty()){
        QFont font(QFontDatabase::applicationFontFamilies(fontId).first());
        font.setStyleStrategy(QFont::PreferAntialias);
        font4log = font;
    }


    QcMainWindow w(font4log, defFontPointSize);
    w.show();

    return a.exec();
}
