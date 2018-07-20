#ifndef DATABASESETTINGS_H
#define DATABASESETTINGS_H

#include "src/shared/matildaconfwidget.h"

namespace Ui {
class DatabaseSettings;
}

class DatabaseSettings : public MatildaConfWidget
{
    Q_OBJECT

public:
    explicit DatabaseSettings(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent = 0);
    ~DatabaseSettings();

public slots:
    void initPage();

    void saveSettings();


private:
    Ui::DatabaseSettings *ui;
};

#endif // DATABASESETTINGS_H
