#include "metersdatetime.h"
#include "ui_metersdatetime.h"
#include "map-pgs/mapwidget.h"
#include "src/matilda/settloader.h"

MetersDateTime::MetersDateTime(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    ReferenceWidgetClass(lDevInfo, gHelper, gSett4all, parent),
    ui(new Ui::MetersDateTime)
{
    ui->setupUi(this);
    isMapReady = false;

}

MetersDateTime::~MetersDateTime()
{
    delete ui;
}

void MetersDateTime::clearPage()
{

}

void MetersDateTime::setPageSett(const QVariantHash &h)
{

}

void MetersDateTime::onModelChanged()
{

}

void MetersDateTime::initPage()
{
    setupObjects(ui->tvTable, ui->tbFilter, ui->cbFilterMode, ui->leFilter, SETT_FILTERS_METERDATETIME);
    GuiHelper::modelSetHorizontalHeaderItems(model, QStringList());

}

void MetersDateTime::on_tbShowList_clicked()
{
    ui->tbShowList->setChecked(true);
    ui->stackedWidget->setCurrentIndex(0);
    ui->tbShowMap->setChecked(false);
}

void MetersDateTime::on_tbShowMap_clicked()
{
    ui->tbShowList->setChecked(false);
    ui->stackedWidget->setCurrentIndex(1);
    ui->tbShowMap->setChecked(true);

    if(!isMapReady){
        isMapReady = true;

        QTimer *tmrModelChanged = new QTimer(this);
        tmrModelChanged->setInterval(555);
        tmrModelChanged->setSingleShot(true);


        proxy_model->enableModelChangedSignal();
        connect(proxy_model, SIGNAL(onModelChanged()), tmrModelChanged, SLOT(start()) );
        connect(tmrModelChanged, SIGNAL(timeout()), this, SLOT(onModelChanged()) );

        MapWidget *w = new MapWidget(true);
        ui->vlQmlMap->addWidget(w);

        connect(this, SIGNAL(showMapEs(QString))  , w, SLOT(showMap(QString)) );

        connect(this, SIGNAL(setNewDeviceModelEs(QVariantList)), w, SIGNAL(setNewDeviceModel(QVariantList)) );
        connect(this, SIGNAL(showThisDeviceNIEs(QString))      , w, SIGNAL(showThisDeviceNI(QString)) );
        //        connect(this, SIGNAL(showThisCoordinatesEs(QString))   , w, SIGNAL(showThisCoordinates(QString)) );

        //        connect(w, SIGNAL(showThisDev(QString)), this, SLOT(showThisDev(QString)) );
        //        connect(w, SIGNAL(addDevice(QString))  , this, SLOT(addDevice(QString))   );
        //        connect(w, SIGNAL(updateModel4ls())    , this, SLOT(onModelChanged())     );
        //        connect(w, SIGNAL(removeDevice(QString)), this, SLOT(removeDevice(QString)));
        //        connect(w, SIGNAL(moveDevice(QVariantHash)), this, SLOT(moveDevice(QVariantHash)));
        //        connect(w, SIGNAL(addDeviceStreet(QVariantHash)), this, SLOT(addDeviceStreet(QVariantHash)));

//        connect(ui->tbShowMap, SIGNAL(toggled(bool)), w, SLOT(isParentWidgetVisible(bool)) );

        QTimer::singleShot(11, this, SLOT(onModelChanged()) );
    }

    emit showMapEs(gHelper->getLastLang());
}
