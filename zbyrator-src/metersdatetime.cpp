#include "metersdatetime.h"
#include "ui_metersdatetime.h"
#include "map-pgs/mapwidget.h"
#include "gui-src/settloader.h"
#include "src/widgets/selectionchecker.h"
#include "gui-src/showmesshelper4wdgt.h"
#include "src/meter/definedpollcodes.h"
#include "src/zbyrator-v2/quickpollhelper.h"


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

QVariant MetersDateTime::getPageSett4read(bool &ok, QString &mess)
{
    Q_UNUSED(ok);
    Q_UNUSED(mess);
    QTimer::singleShot(1, this, SLOT(onPbReadAll_clicked()));
    return QVariantHash();
}

void MetersDateTime::clearPage()
{

}

void MetersDateTime::setPageSett(const MyListStringList &listRows, const QVariantMap &col2data, const QStringList &headerH, const QStringList &header, const bool &hasHeader)
{

    const QString currNi = headerH.contains("NI") ? TableViewHelper::getCellValueOfcurrentRow(ui->tvTable, headerH.indexOf("NI")) : "";

    StandardItemModelHelper::append2model(listRows, col2data, headerH, header, hasHeader, model);

    if(!currNi.isEmpty())
        TableViewHelper::selectRowWithThisCell(ui->tvTable, currNi, headerH.indexOf("NI"));

    ui->widget->setDisabled(listRows.isEmpty());

    setHasDataFromRemoteDevice();

    emit resizeTv2content(ui->tvTable);
}


void MetersDateTime::onModelChanged()
{

}

void MetersDateTime::meterDateTimeDstStatus(QString ni, QDateTime dtLocal, QString stts)
{
    if(ni.isEmpty())
        return;

    const int row = StandardItemModelHelper::getRowFromNI(4, ni, model);
    if(row < 0)
        return;

    stts.append("\n\n");
    const QStringList l = stts.split("\n");

    model->item(row, 0)->setText(dtLocal.toString(lastDateTimeMask));
    model->item(row, 1)->setText(l.at(0));
    model->item(row, 6)->setText(l.at(1));

    TableViewHelper::selectRowWithThisCell(ui->tvTable, ni, 4);

    resizeLastTv2content();
}

void MetersDateTime::initPage()
{

    setupObjects(ui->tvTable, ui->tbFilter, ui->cbFilterMode, ui->leFilter, SETT_FILTERS_METERDATETIME);
    StandardItemModelHelper::modelSetHorizontalHeaderItems(model, QStringList());

    ui->widget_2->setEnabled(false);
    ui->widget->setEnabled(false);

    connect(gHelper, SIGNAL(setPbWriteEnableDisable(bool)), ui->pbRead, SLOT(setDisabled(bool)));
    connect(gHelper, SIGNAL(setPbWriteEnableDisable(bool)), ui->pbWrite, SLOT(setDisabled(bool)));
    connect(gHelper, SIGNAL(setPbWriteEnableDisable(bool)), ui->pbCorrectionAll, SLOT(setDisabled(bool)));

    ui->pbRead->setDisabled(gHelper->managerEnDisBttn.pbWriteDis);
    ui->pbWrite->setDisabled(gHelper->managerEnDisBttn.pbWriteDis);
    ui->pbCorrectionAll->setDisabled(gHelper->managerEnDisBttn.pbWriteDis);


    SelectionChecker *tmr = new SelectionChecker(this);
    tmr->setWatchTable(ui->tvTable, ui->widget_2);
    ui->widget_3->setVisible(false);
    connect(tmr, &SelectionChecker::setWdgtEnable, ui->widget_3, &QWidget::setVisible);

    emit onReloadAllMeters();


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

void MetersDateTime::on_tvTable_customContextMenuRequested(const QPoint &pos)
{
    gHelper->createCustomMenu(pos, ui->tvTable, (GuiHelper::ShowReset|GuiHelper::ShowExport|GuiHelper::ShowOnlyCopy), CLBRD_SMPL_PRXTBL, ShowMessHelper4wdgt::matildaFileName(windowTitle()));

}

void MetersDateTime::onPbReadAll_clicked()
{
    startOperation(TableViewHelper::getRowsText(ui->tvTable, 4), POLL_CODE_READ_DATE_TIME_DST);
}

void MetersDateTime::on_pbCorrectionAll_clicked()
{
    startOperation(TableViewHelper::getRowsText(ui->tvTable, 4), POLL_CODE_WRITE_DATE_TIME);
}

void MetersDateTime::on_pbRead_clicked()
{
    startOperation(TableViewHelper::selectedRowText(ui->tvTable, 4), POLL_CODE_READ_DATE_TIME_DST);
}

void MetersDateTime::on_pbWrite_clicked()
{
    startOperation(TableViewHelper::selectedRowText(ui->tvTable, 4), POLL_CODE_WRITE_DATE_TIME);

}

void MetersDateTime::startOperation(const QStringList &listni, const quint8 &operation)
{
    if(gHelper->managerEnDisBttn.pbReadDis)
        return;

    if(listni.isEmpty()){
        gHelper->showMessSlot(tr("no meters"));
        return;
    }

    QString mess;
    const QVariantMap map = QuickPollHelper::createPollMap4relay(listni, 0, mess);

    if(map.isEmpty())
        return;

    ui->tvTable->clearSelection();
    StandardItemModelHelper::clearCells(listni, 4,QList<int>() << 0 << 1 << 6, model);
    gHelper->updateSettDateMaskAndDotPos();
    emit setLastPageId(accessibleName());
    lastDateTimeMask = gHelper->dateMask + " hh:mm:ss";
    emit command4dev(operation, map);
}
