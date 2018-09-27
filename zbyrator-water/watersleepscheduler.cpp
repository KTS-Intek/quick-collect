#include "watersleepscheduler.h"
#include "ui_watersleepscheduler.h"

#include "zbyrator-water/src/watersleepschedulesaver.h"
#include "waterprofilewdgt.h"
#include <QMessageBox>
#include "src/meter/definedpollcodes.h"
#include "src/widgets/selectionchecker.h"
#include "src/zbyrator-v2/quickpollhelper.h"
#include <QJsonDocument>
#include "src/meter/zbyratorhelper.h"
#include "src/matilda/settloader.h"

//відображення зчитаного розкладу з лічильників
//запис розкладу до лічильників

WaterSleepScheduler::WaterSleepScheduler(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    ReferenceWidgetClass(lDevInfo, gHelper, gSett4all, parent),
    ui(new Ui::WaterSleepScheduler)
{
    ui->setupUi(this);
}

WaterSleepScheduler::~WaterSleepScheduler()
{
    delete ui;
}

QVariant WaterSleepScheduler::getPageSett4read(bool &ok, QString &mess)
{
    Q_UNUSED(ok);
    Q_UNUSED(mess);
    QTimer::singleShot(1, this, SLOT(onPbReadAll_clicked()));
    return QVariantHash();
}

void WaterSleepScheduler::initPage()
{
    ui->widget_2->setEnabled(false);
    setupObjects(ui->tvTable, ui->tbFilter, ui->cbFilterMode, ui->leFilter, SETT_FILTERS_ZBYR_WTR_SCHDL);
    emit onReloadAllMeters();


    WaterProfileWdgt *profileWdgt = new WaterProfileWdgt(false, this);
    ui->verticalLayout_2->addWidget(profileWdgt);
    connect(this, SIGNAL(setProfile(QVariantHash)), profileWdgt, SLOT(setProfile(QVariantHash)));

    connect(ui->tvTable, SIGNAL(clicked(QModelIndex)), this, SLOT(onTvTable_clicked(QModelIndex)));

    profileWdgt->setMaximumHeight(ui->cbxProfile->height() * 4);


    EditWaterProfile *editWdgt = new EditWaterProfile(this);
    connect(this, SIGNAL(setProfileEdit(QString,QVariantHash)), editWdgt, SLOT(setProfileSlot(QString,QVariantHash)));
    connect(editWdgt, SIGNAL(onSaveProfile(QString,QVariantHash)), this, SLOT(onSaveProfile(QString,QVariantHash)) );
    connect(editWdgt, SIGNAL(deleteProfileName()), this, SLOT(deleteProfileName()) );

    SelectionChecker *tmr = new SelectionChecker(this);
    tmr->setWatchTable(ui->tvTable, ui->widget_2);

    connect(tmr, &SelectionChecker::setWdgtEnable, ui->widget_2, &QWidget::setEnabled);
    connect(tmr, &SelectionChecker::setSelectedCount, this, &WaterSleepScheduler::setSelectedCount);


    QTimer::singleShot(11, editWdgt, SLOT(setupPage()) );


    connect(gHelper, SIGNAL(setPbWriteEnableDisable(bool)), ui->pbRead, SLOT(setDisabled(bool)));
    connect(gHelper, SIGNAL(setPbWriteEnableDisable(bool)), ui->pbWrite, SLOT(setDisabled(bool)));

    ui->pbRead->setDisabled(gHelper->managerEnDisBttn.pbWriteDis);
    ui->pbWrite->setDisabled(gHelper->managerEnDisBttn.pbWriteDis);


   QTimer *t = new QTimer(this);
   t->setInterval(333);
   t->setSingleShot(true);
   connect(this, SIGNAL(startShowCurrProfileTmr()), t, SLOT(start()) );
   connect(ui->cbxProfile, SIGNAL(currentIndexChanged(int)), t, SLOT(start()) );

   connect(t, SIGNAL(timeout()), this, SLOT(onShowCurProfileTmr()) );

   updateSleepProfilesSett();




}

void WaterSleepScheduler::clearPage()
{

}

void WaterSleepScheduler::setPageSett(const MyListStringList &listRows, const QVariantMap &col2data, const QStringList &headerH, const QStringList &header, const bool &hasHeader)
{//
    const QString currNi = headerH.contains("NI") ? TableViewHelper::getCellValueOfcurrentRow(ui->tvTable, headerH.indexOf("NI")) : "";

    StandardItemModelHelper::append2model(listRows, col2data, headerH, header, hasHeader, model);

    if(!currNi.isEmpty())
        TableViewHelper::selectRowWithThisCell(ui->tvTable, currNi, headerH.indexOf("NI"));

    ui->widget->setDisabled(listRows.isEmpty());

    setHasDataFromRemoteDevice();

    emit resizeTv2content(ui->tvTable);
    QTimer::singleShot(111, this, SLOT(updateSleepProfilesSett()));

}

void WaterSleepScheduler::waterMeterSchedulerStts(QString ni, QDateTime dtLocal, QString stts, QVariantHash sheduler)
{
    if(ni.isEmpty())
        return;

    const int row = StandardItemModelHelper::getRowFromNI(4, ni, model);
    if(row < 0)
        return;

//    stts.append("\n\n");
//    const QStringList l = stts.split("\n");



    model->item(row, 0)->setText(dtLocal.toString(lastDateTimeMask));

    if(!sheduler.isEmpty()){
         const int indx = ui->cbxProfile->findData(sheduler);
         stts = (indx < 1) ?
                 QString(QJsonDocument(QJsonObject::fromVariantHash(sheduler)).toJson(QJsonDocument::Compact)) :
                 ui->cbxProfile->itemText(indx);

    }

    model->item(row, 1)->setText(stts);
    model->item(row, 1)->setData(sheduler, Qt::UserRole + 1);

//    model->item(row, 6)->setText(l.at(1));

    TableViewHelper::selectRowWithThisCell(ui->tvTable, ni, 4);

    resizeLastTv2content();
}

void WaterSleepScheduler::onSaveProfile(QString name, QVariantHash profile)
{
    //save profile

    if(name.isEmpty() || ui->cbxProfile->itemText(0) == name ){
        emit showMess(tr("bad profile name("));
        return;
    }

    if(profile.isEmpty()){
        emit showMess(tr("bad profile settings("));
        return;
    }


    updateSleepProfilesSett();
    const int indx = ui->cbxProfile->findText(name);// Data(QVariant::fromValue(profile));

    if(indx > 0 && (profile2delete.isEmpty() || (!profile2delete.isEmpty() && profile2delete != name))){

        if(QMessageBox::question(this, tr("Overwrite"), tr("The profile with the name '%1' is already exists. Do you want to replace it?")) != QMessageBox::Yes)
            return;

        updateSleepProfilesSett();//to prevent bad situations
    }
    if(name == profile2delete)
        profile2delete.clear();

//    if(!profile2delete.isEmpty()){ I think that it is unusful
//        indx = ui->cbxProfile->findText(profile2delete);
//        if(indx > 0)
//            ui->cbxProfile->removeItem(indx);
//    }

    ui->cbxProfile->addItem(name, profile);

//save to memory
    WaterSleepScheduleSaver::addNewProfile(name, profile);

}



void WaterSleepScheduler::updateSleepProfilesSett()
{    
    updatetSleepProfiles(WaterSleepScheduleSaver::getSavedSett());
}

void WaterSleepScheduler::deleteProfileName()
{
    if(profile2delete.isEmpty())
        return;

    if(QMessageBox::question(this, tr("Delete"), tr("Do you want to delete the profile '%1'?").arg(profile2delete)) != QMessageBox::Yes)
        return;

    updateSleepProfilesSett();
    const int indx = ui->cbxProfile->findText(profile2delete);
    if(indx < 1)
        return;

    ui->cbxProfile->removeItem(indx);
    //save cbx 2 memory
    WaterSleepScheduleSaver::removeOneProfile(profile2delete);

}

void WaterSleepScheduler::setSelectedCount(int selectedItems)
{
    ui->pbWrite->setText( (selectedItems < 1) ? tr("Write") : tr("Write (%1)").arg(selectedItems));
}


void WaterSleepScheduler::on_tbAdd_clicked()
{
    profile2delete.clear();
    emit setProfileEdit(profile2delete, ui->cbxProfile->currentData().toHash());// ui->cbxProfile->currentData().toHash());
}

void WaterSleepScheduler::on_tvTable_customContextMenuRequested(const QPoint &pos)
{

}

void WaterSleepScheduler::startOperation(const QStringList &listni, const quint8 &pollCode)
{
    if(gHelper->managerEnDisBttn.pbReadDis)
        return;

    if(listni.isEmpty()){
        emit showMess(tr("no meters"));
        return;
    }

    const QVariantHash params = (ui->cbxProfile->currentIndex() < 1) ? QVariantHash()  : ui->cbxProfile->currentData().toHash();
    if(pollCode == POLL_CODE_WRITE_WTR_SLEEP && params.isEmpty()){
        emit showMess(tr("bad profile"));
        return;
    }

    QString mess;
    const QVariantMap map = QuickPollHelper::createPollMapWithParams(listni, params.isEmpty() ? "-" : ZbyratorHelper::getSleepProfileLine(params), mess);

    if(map.isEmpty())
        return;

    ui->tvTable->clearSelection();
    StandardItemModelHelper::clearCells(listni, 4,QList<int>() << 0 << 1, model);
    gHelper->updateSettDateMaskAndDotPos();
    emit setLastPageId(accessibleName());
    lastDateTimeMask = gHelper->dateMask + " hh:mm:ss";
    emit command4dev(pollCode, map);

}

void WaterSleepScheduler::onTvTable_clicked(const QModelIndex &index)
{
    const int row = proxy_model->mapToSource(index).row();
    if(row < 0)
        return;

    const QVariant currMeter = model->item(row, 1)->data(Qt::UserRole + 1);


    const int indx = ui->cbxProfile->findData(currMeter);
    if(indx < 0){
        ui->cbxProfile->setItemData(0, currMeter.isValid() ? currMeter : WaterSleepScheduleSaver::getDefaultProfileVar());
        ui->cbxProfile->setItemData(0, QString("%1 %2 %3").arg(model->item(row, 4)->text()).arg(model->item(row, 3)->text()).arg(model->item(row, 2)->text()), Qt::UserRole + 2);
    }
    ui->cbxProfile->setCurrentIndex(qMax(0, indx));
    emit startShowCurrProfileTmr();

}

void WaterSleepScheduler::onShowCurProfileTmr()
{
//to waterprofilewdgt
    emit setProfile(ui->cbxProfile->currentData().toHash());
    ui->pbWrite->setEnabled(ui->cbxProfile->currentIndex() > 0);
}

void WaterSleepScheduler::updatetSleepProfiles(const QVariantHash &profileName2schedule)
{
    QString currTxt;
    QVariant currData;
    if(!ui->cbxProfile->currentText().isEmpty()){
        currData = ui->cbxProfile->currentData();
        currTxt = ui->cbxProfile->currentText();
    }

    const QVariant currentMeterData = (ui->cbxProfile->count() > 0) ? ui->cbxProfile->itemData(0) : QVariant();


    QList<QString> listK = profileName2schedule.keys();
    std::sort(listK.begin(), listK.end());
    ui->cbxProfile->clear();

    ui->cbxProfile->addItem(tr("<Current Meter>"), currentMeterData);

    int indxByTxt = -1, indxByData = -1;
    const int lksize = listK.size();
    for(int i = 0, imax = lksize; i < imax; i++){

        ui->cbxProfile->addItem(listK.at(i), profileName2schedule.value(listK.at(i)));
//        ui->cbxProfile->setItemData(i, profileName2schedule.value(listK.at(i)) , Qt::ToolTipRole );

        if(indxByTxt < 0 && !currTxt.isEmpty() && listK.at(i) == currTxt)
            indxByTxt = i;
        if(indxByData < 0 && currData.isValid() && profileName2schedule.value(listK.at(i)) == currData)
            indxByData = i;
    }

    if(indxByTxt >= 0)
        ui->cbxProfile->setCurrentIndex(indxByTxt);
    else if(indxByData >= 0)
        ui->cbxProfile->setCurrentIndex(indxByData);
    else
        ui->cbxProfile->setCurrentIndex(0);

    ui->pbWrite->setEnabled(lksize > 0);
//    ui->pbRead->setEnabled(lk);
}

void WaterSleepScheduler::on_tbEdit_clicked()
{
    profile2delete = (ui->cbxProfile->currentIndex() < 1) ? "" : ui->cbxProfile->currentText();
    emit setProfileEdit((ui->cbxProfile->currentIndex() < 1) ? ui->cbxProfile->currentData(Qt::UserRole + 2).toString() : ui->cbxProfile->currentText(), ui->cbxProfile->currentData().toHash());
}


void WaterSleepScheduler::onPbReadAll_clicked()
{
    startOperation(TableViewHelper::getRowsText(ui->tvTable, 4), POLL_CODE_READ_WTR_SLEEP);
}

void WaterSleepScheduler::on_pbWrite_clicked()
{
    startOperation(TableViewHelper::selectedRowText(ui->tvTable, 4), POLL_CODE_WRITE_WTR_SLEEP);

}

void WaterSleepScheduler::on_pbRead_clicked()
{
    startOperation(TableViewHelper::selectedRowText(ui->tvTable, 4), POLL_CODE_READ_WTR_SLEEP);
}
