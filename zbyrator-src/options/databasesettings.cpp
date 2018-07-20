#include "databasesettings.h"
#include "ui_databasesettings.h"

#include "src/matilda/settloader.h"
#include "matilda-bbb-src/shared/pathsresolver.h"
#include "src/matilda/matildalimits.h"
#include <QTimer>

DatabaseSettings::DatabaseSettings(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    MatildaConfWidget(lDevInfo, gHelper, gSett4all, parent),
    ui(new Ui::DatabaseSettings)
{
    ui->setupUi(this);
}

DatabaseSettings::~DatabaseSettings()
{
    delete ui;
}

void DatabaseSettings::initPage()
{
    ui->lePathToDb->setText(PathsResolver::path2database());

    ui->sbLimitDBsize->setValue( SettLoader::loadSett(SETT_OPTIONS_DB_MAXSIZE, MAX_DATABASE_SIZE).toInt() / 1000000);//550 000 000 //~550M


    QTimer *t = new QTimer(this);
    t->setSingleShot(true);
    t->setInterval(555);
    connect(ui->sbLimitDBsize, SIGNAL(valueChanged(int)), t, SLOT(start()));
    connect(t, SIGNAL(timeout()), this, SLOT(saveSettings()));
}

void DatabaseSettings::saveSettings()
{
    if((SettLoader::loadSett(SETT_OPTIONS_DB_MAXSIZE, MAX_DATABASE_SIZE + 1).toInt() / 1000000) != ui->sbLimitDBsize->value())
        SettLoader::saveSett(SETT_OPTIONS_DB_MAXSIZE, ui->sbLimitDBsize->value() * 1000000);
}
