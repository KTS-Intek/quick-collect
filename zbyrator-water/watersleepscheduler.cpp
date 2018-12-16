#include "watersleepscheduler.h"
#include "ui_watersleepscheduler.h"

#include "zbyrator-water/src/watersleepschedulesaver.h"
#include "waterprofilewdgt.h"
#include <QMessageBox>
#include "src/meter/definedpollcodes.h"
#include "src/widgets/selectionchecker.h"

#ifdef IS_ZBYRATOR
#include "src/zbyrator-v2/quickpollhelper.h"
#endif

#include <QJsonDocument>
#include "gui-src/settloader.h"
#include "zbyrator-water/waterscheduleeditorpro.h"
#include "gui-src/showmesshelper4wdgt.h"
#include "src/zbyrator-v2/watermeterhelper.h"
#include "src/zbyrator-v2/activesleepprofileshelper.h"
#include "src/matilda-conf/protocol5togui.h"
#include "src/matilda/moji_defy.h"

#include "watersleepscheduleradder.h"

//відображення зчитаного розкладу з лічильників
//запис розкладу до лічильників

//------------------------------------------------------------------------

WaterSleepScheduler::WaterSleepScheduler(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    ReferenceWidgetClass(lDevInfo, gHelper, gSett4all, parent),
    ui(new Ui::WaterSleepScheduler)
{
    ui->setupUi(this);

#ifndef IS_ZBYRATOR
    ui->pbRead->hide();
    ui->pbWrite->setText(tr("Apply"));
#endif

    ui->tbShowList->hide();
    ui->tbShowMap->hide();



}

//------------------------------------------------------------------------

WaterSleepScheduler::~WaterSleepScheduler()
{
    delete ui;
}

//------------------------------------------------------------------------

QVariant WaterSleepScheduler::getPageSett4read(bool &ok, QString &mess)
{
    ok = true;
    mess = "";
    gHelper->updateSettDateMaskAndDotPos();
    lastDateTimeMask = gHelper->dateMask + " hh:mm:ss";
    gHelper->clearUndoHistory(ui->tvTable);
#ifdef IS_ZBYRATOR
    QTimer::singleShot(1, this, SLOT(onPbReadAll_clicked()));
    return QVariantHash();
#else
    return GuiHelper::startReadFramedList();
#endif
}

//------------------------------------------------------------------------

QVariantHash WaterSleepScheduler::getPageSett(bool &ok, QString &mess, const bool &is4copy)
{
    Q_UNUSED(is4copy);
    bool eraseAll;

    QMap<QString,QString> map;
    int watchCol = -1;
//    if(meterType == UC_METER_WATER){
//        map = WaterSleepScheduleSaver::getSavedSettMap();
//        watchCol = getHeaderKeys4meterType().split(",").indexOf("prof");
//    }
    //You shoud convert UCon cols to UC cols!!!!
    const QVariantHash h = StandardItemModelHelper::model2remoteExt(ActiveSleepProfilesHelper::getHeader4wtrLastProfile4ucon(), model, map, watchCol, true, eraseAll);
//    h.insert("lastDateTimeMask", lastDateTimeMask);
    ok = true;
    mess = eraseAll ? tr("Do you really want to clear the list of the last active profiles?") : "";
    return h;
}

//------------------------------------------------------------------------

void WaterSleepScheduler::initPage()
{
    ui->widget_2->setEnabled(false);
    setupObjects(ui->tvTable, ui->tbFilter, ui->cbFilterMode, ui->leFilter, SETT_FILTERS_ZBYR_WTR_SCHDL);
    emit onReloadAllMeters();
    lastTvHighlight.key = "Name";

    setupTbShowHide(ui->tbShowHide, ui->wdgtOperations, true, true);
    setupTbShowHide(ui->tbShowHide, ui->widget, true, true);

    WaterProfileWdgt *profileWdgt = new WaterProfileWdgt(false, this);
    ui->verticalLayout_2->addWidget(profileWdgt);
    connect(this, SIGNAL(setProfile(QVariantHash)), profileWdgt, SLOT(setProfile(QVariantHash)));

    connect(ui->tvTable, SIGNAL(clicked(QModelIndex)), this, SLOT(onTvTable_clicked(QModelIndex)));

    profileWdgt->setMaximumHeight(ui->cbxProfile->height() * 4);



    SelectionChecker *tmr = new SelectionChecker(this);
    tmr->setWatchTable(ui->tvTable, ui->widget_2);

    connect(tmr, &SelectionChecker::setWdgtEnable, ui->widget_2, &QWidget::setEnabled);
    connect(tmr, &SelectionChecker::setSelectedCount, this, &WaterSleepScheduler::setSelectedCount);

#ifdef IS_ZBYRATOR

    connect(gHelper, SIGNAL(setPbWriteEnableDisable(bool)), ui->pbRead, SLOT(setDisabled(bool)));
    connect(gHelper, SIGNAL(setPbWriteEnableDisable(bool)), ui->pbWrite, SLOT(setDisabled(bool)));

    ui->pbRead->setDisabled(gHelper->managerEnDisBttn.pbWriteDis);
    ui->pbWrite->setDisabled(gHelper->managerEnDisBttn.pbWriteDis);

#endif

   QTimer *t = new QTimer(this);
   t->setInterval(333);
   t->setSingleShot(true);
   connect(this, SIGNAL(startShowCurrProfileTmr()), t, SLOT(start()) );
   connect(ui->cbxProfile, SIGNAL(currentIndexChanged(int)), t, SLOT(start()) );

   connect(t, SIGNAL(timeout()), this, SLOT(onShowCurProfileTmr()) );

   updateSleepProfilesSett();

   connect(this, SIGNAL(settReceivedWithData()), this, SLOT(onSettReceivedWithData()));

   ui->tvTable->setAccessibleName("4");

   emit reloadSavedSleepProfiles();
   readDefCommandOnUpdate();

   connect(ui->pbAddNi, SIGNAL(clicked(bool)), this, SLOT(checkCreateAddWdgt()));
   connect(this, &WaterSleepScheduler::setEditNiPageSett, this, &WaterSleepScheduler::createAddWdgt);


   connectModelSignalWithGuiHelper(CLBRD_WMETERLASTPRF);

}

//------------------------------------------------------------------------

void WaterSleepScheduler::clearPage()
{
    model->clear();
    ui->tbFilter->setEnabled(false);
    ui->leFilter->clear();
}

//------------------------------------------------------------------------

void WaterSleepScheduler::setPageSett(const QVariantHash &h)
{
    if(h.value("hasHeader").toBool())
        ui->tbFilter->setEnabled(true);
    StandardItemModelHelper::append2model(h, model);
    setHasDataFromRemoteDevice();
    emit resizeTv2content(ui->tvTable);
//    QTimer::singleShot(1, this, SLOT(updatePowerCenters()));
}

//------------------------------------------------------------------------
#ifdef IS_ZBYRATOR

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
#endif
//------------------------------------------------------------------------

void WaterSleepScheduler::waterMeterSchedulerStts(QString ni, QDateTime dtLocal, QString stts, QVariantHash sheduler)
{
    if(ni.isEmpty())
        return;

    const int row = StandardItemModelHelper::getRowFromNI(4, ni, model);
    if(row < 0)
        return;

//    stts.append("\n\n");
//    const QStringList l = stts.split("\n");

    if(lastDateTimeMask.isEmpty()){
        gHelper->updateSettDateMaskAndDotPos();
        lastDateTimeMask = gHelper->dateMask + " hh:mm:ss";
    }



    model->item(row, 0)->setText(dtLocal.toString(lastDateTimeMask));
    QString profname = stts;
    if(!sheduler.isEmpty()){
         const int indx = ui->cbxProfile->findData(sheduler);
         profname = (indx < 1) ?
                 "" : // QString(QJsonDocument(QJsonObject::fromVariantHash(sheduler)).toJson(QJsonDocument::Compact)) :
                 ui->cbxProfile->itemText(indx);

    }

    const QString profilelinejson = Protocol5toGUI::makeSleepProfileLine4waterMeters(profname, sheduler);

    model->item(row, 1)->setText(profilelinejson);// const QString &proflinesmpl));//  stts);
//    model->item(row, 1)->setData(sheduler, Qt::UserRole + 1);

//    model->item(row, 6)->setText(l.at(1));

    TableViewHelper::selectRowWithThisCell(ui->tvTable, ni, 4);

    resizeLastTv2content();
}

//------------------------------------------------------------------------


void WaterSleepScheduler::updateSleepProfilesSett()
{    
    updatetSleepProfiles(WaterSleepScheduleSaver::getSavedSett());
}


//------------------------------------------------------------------------


void WaterSleepScheduler::setSelectedCount(int selectedItems)
{
#ifdef IS_ZBYRATOR
    ui->pbWrite->setText( (selectedItems < 1) ? tr("Write") : tr("Write (%1)").arg(selectedItems));
#else
    ui->pbWrite->setText( (selectedItems < 1) ? tr("Apply") : tr("Apply (%1)").arg(selectedItems));
#endif
}

//------------------------------------------------------------------------

void WaterSleepScheduler::openEditWidget(const bool &editMode)
{
    WaterScheduleEditorPro *w = new WaterScheduleEditorPro(lDevInfo, gHelper, gSett4all, this);
    connect(w, SIGNAL(updateProfileSett()), this, SLOT(updateSleepProfilesSett()));
    gHelper->addWdgt2stackWdgtSlot(w, WDGT_TYPE_EDITENERGYWIDGET);

    if(editMode){
        if(ui->cbxProfile->currentIndex() == 0){
            w->setEditProfileFromMeter(ui->cbxProfile->currentData().toHash().isEmpty() ? WaterSleepScheduleSaver::getDefaultProfile() : ui->cbxProfile->currentData().toHash());
        }else
            w->setEditProfileName(ui->cbxProfile->currentText());
    }else{
        w->setEditProfileFromMeter(WaterSleepScheduleSaver::getDefaultProfile());
    }
}

//------------------------------------------------------------------------

void WaterSleepScheduler::onSettReceivedWithData()
{
#ifdef IS_ZBYRATOR
    ui->widget->setDisabled(false);
#endif
}


//------------------------------------------------------------------------

void WaterSleepScheduler::on_tbAdd_clicked()
{
    openEditWidget(false);
}
//------------------------------------------------------------------------
void WaterSleepScheduler::on_tvTable_customContextMenuRequested(const QPoint &pos)
{
    QList<QAction*> la;
#ifdef IS_ZBYRATOR
   if(
        #else
    if((gHelper->devRghts.youAreOper || gHelper->devRghts.youAreRoot) && lDevInfo->matildaDev.protocolVersion >= MATILDA_PROTOCOL_VERSION_V5  &&
#endif
            !ui->tvTable->selectionModel()->selectedRows().isEmpty()){


        QAction *actDs = new QAction(tr("Delete selected") , this);
        actDs->setIcon(QIcon(":/katynko/svg3/user-trash-full.svg"));
        actDs->setEnabled(true);
        connect(actDs, SIGNAL(triggered(bool)), this, SLOT(deleteSelected()));// gHelper, SLOT(onDeleteSelectedAct()) );
//        connect(actDs, SIGNAL(triggered(bool)), this, SLOT(sayModelChangedLater()) );

        la.append(actDs);



    }
    ui->tvTable->setAccessibleName("4");


    gHelper->createCustomMenu(pos, ui->tvTable, (GuiHelper::ShowReset|GuiHelper::ShowExport|GuiHelper::ShowCopy|GuiHelper::ShowUndoAct), CLBRD_WMETERLASTPRF, ShowMessHelper4wdgt::matildaFileName(windowTitle()), la);
}

//------------------------------------------------------------------------

#ifdef IS_ZBYRATOR
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
    const QVariantMap map = QuickPollHelper::createPollMapWithParams(listni, params.isEmpty() ? "-" : WaterMeterHelper::oneProfile2lineSmpl(params), mess);

    if(map.isEmpty())
        return;

    ui->tvTable->clearSelection();
    StandardItemModelHelper::clearCells(listni, 4,QList<int>() << 0 << 1, model);
    gHelper->updateSettDateMaskAndDotPos();
    emit setLastPageId(accessibleName());
    lastDateTimeMask = gHelper->dateMask + " hh:mm:ss";
    emit command4dev(pollCode, map);

}

void WaterSleepScheduler::onPbReadAll_clicked()
{
    startOperation(TableViewHelper::getRowsText(ui->tvTable, 4), POLL_CODE_READ_WTR_SLEEP);
}



void WaterSleepScheduler::on_pbRead_clicked()
{
    startOperation(TableViewHelper::selectedRowText(ui->tvTable, 4), POLL_CODE_READ_WTR_SLEEP);
}

#endif

//------------------------------------------------------------------------

void WaterSleepScheduler::on_pbWrite_clicked()
{
#ifdef IS_ZBYRATOR
    startOperation(TableViewHelper::selectedRowText(ui->tvTable, 4), POLL_CODE_WRITE_WTR_SLEEP);
#else
//apply
    if(lastDateTimeMask.isEmpty()){
        gHelper->updateSettDateMaskAndDotPos();
        lastDateTimeMask = gHelper->dateMask + " hh:mm:ss";
    }

    const QString dtLocal = QDateTime::currentDateTime().toString(lastDateTimeMask);
    const QVariantHash sheduler = ui->cbxProfile->currentData().toHash();
    if(sheduler.isEmpty()){
        emit showMess(tr("Settings are broken"));
        return;
    }

    const QList<int> lindx = TableViewHelper::getSelectedRows(ui->tvTable, 0, false);
    const QString settline = Protocol5toGUI::makeSleepProfileLine4waterMeters(ui->cbxProfile->currentText(), sheduler);

    for(int i = 0, imax = lindx.size(); i < imax; i++){

        const int row = lindx.at(i);
        if(settline == model->item(row, 1)->text())
            continue;


        model->item(row, 0)->setText(dtLocal);
        model->item(row, 1)->setText(settline);// const QString &proflinesmpl));//  stts);
        model->item(row,7)->setText("1");

        QFont f = model->item(row,0)->font();
        f.setBold(true);
        model->item(row, 0)->setFont(f);
    }
    resizeLastTv2content();

#endif

}

//------------------------------------------------------------------------

void WaterSleepScheduler::onTvTable_clicked(const QModelIndex &index)
{
    const int row = proxy_model->mapToSource(index).row();
    if(row < 0)
        return;

    const QVariant currMeter = WaterMeterHelper::oneProfileFromLineSettVarHash(Protocol5toGUI::getProfLineFromTableCell(model->item(row, 1)->text()));// model->item(row, 1)->data(Qt::UserRole + 1);

    const int indx = ui->cbxProfile->findData(currMeter);
    if(indx < 0){
        ui->cbxProfile->setItemData(0, currMeter.isValid() ? currMeter : WaterSleepScheduleSaver::getDefaultProfileVar());
        ui->cbxProfile->setItemData(0, QString("%1 %2 %3").arg(model->item(row, 4)->text()).arg(model->item(row, 3)->text()).arg(model->item(row, 2)->text()), Qt::UserRole + 2);
    }
    ui->cbxProfile->setCurrentIndex(qMax(0, indx));
    emit startShowCurrProfileTmr();

}

//------------------------------------------------------------------------

void WaterSleepScheduler::onShowCurProfileTmr()
{
//to waterprofilewdgt
    emit setProfile(ui->cbxProfile->currentData().toHash());
    ui->pbWrite->setEnabled(ui->cbxProfile->currentIndex() > 0);
}

//------------------------------------------------------------------------

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

//------------------------------------------------------------------------

void WaterSleepScheduler::on_tbEdit_clicked()
{
    openEditWidget(true);
}

//------------------------------------------------------------------------

void WaterSleepScheduler::checkCreateAddWdgt()
{
    createAddWdgt("", "", "");
}

//------------------------------------------------------------------------

void WaterSleepScheduler::createAddWdgt(const QString &ni, const QString &profName, const QString &profLineSmpl)
{
    disconnect(ui->pbAddNi, SIGNAL(clicked(bool)), this, SLOT(checkCreateAddWdgt()));
    disconnect(this, &WaterSleepScheduler::setEditNiPageSett, this, &WaterSleepScheduler::createAddWdgt);



    WaterSleepSchedulerAdder *editWdgt = new WaterSleepSchedulerAdder(this);

    connect(ui->pbAddNi, SIGNAL(clicked(bool)), editWdgt, SLOT(addNi()));
    connect(this, &WaterSleepScheduler::setEditNiPageSett, editWdgt, &WaterSleepSchedulerAdder::setPageSett);
    connect(this, &WaterSleepScheduler::setMaps, editWdgt, &WaterSleepSchedulerAdder::setMaps);

    connect(editWdgt, &WaterSleepSchedulerAdder::sendMeNewMaps, this, &WaterSleepScheduler::sendMeNewMaps);
    connect(editWdgt, &WaterSleepSchedulerAdder::onAddNi, this, &WaterSleepScheduler::onAddNi);


    if(lastDateTimeMask.isEmpty()){
        gHelper->updateSettDateMaskAndDotPos();
        lastDateTimeMask = gHelper->dateMask + " hh:mm:ss";
    }

    connect(ui->pbAddNi, SIGNAL(clicked(bool)), editWdgt, SLOT(showLater()));
    connect(this, SIGNAL(setEditNiPageSett(QString,QString,QString)), editWdgt, SLOT(showLater()));

    if(ni.isEmpty()){
        ui->pbAddNi->animateClick();
    }else{
        emit setEditNiPageSett(ni, profName, profLineSmpl);
    }
}

//------------------------------------------------------------------------

void WaterSleepScheduler::sendMeNewMaps()
{

    QMap<QString, QString> ni2model;
    if(true){
        const QList<QString> nikeys = gHelper->waterMeter.hashMeterNi2info.keys();
        for(int i = 0, imax = nikeys.size(); i < imax; i++){
            ni2model.insert(nikeys.at(i), QString("%1, %2")
                            .arg(gHelper->waterMeter.hashMeterNi2info.value(nikeys.at(i)).minfo.model)
                            .arg(gHelper->waterMeter.hashMeterNi2info.value(nikeys.at(i)).minfo.memo));
        }

    }
    QMap<QString, QString> ni2lastProfile;
    if(true){

        const QVariantHash profileName2schedule = WaterSleepScheduleSaver::getSavedSett();

        const QList<QString> listK = profileName2schedule.keys();

        //name ; hash sett -> line smpl actvt=
        for(int i = 0, imax = listK.size(); i < imax; i++)
            ni2lastProfile.insert(listK.at(i), WaterMeterHelper::oneProfile2lineSmpl(profileName2schedule.value(listK.at(i)).toHash()));
    }



    emit setMaps(ni2model, ni2lastProfile);

}

//------------------------------------------------------------------------

void WaterSleepScheduler::onAddNi(QString ni, QString profileName, QString proflineSmpl)
{

    if(ni.isEmpty() || proflineSmpl.isEmpty())
        return;

    const QStringList lastNIs = StandardItemModelHelper::getColText(4, -1, model);



    const QVariantHash profileName2schedule = WaterSleepScheduleSaver::getSavedSett();

    if(profileName2schedule.contains(profileName)){
        proflineSmpl = WaterMeterHelper::oneProfile2lineSmpl(profileName2schedule.value(profileName).toHash());
    }else{
        const QList<QString> listK = profileName2schedule.keys();
        profileName.clear();

        //name ; hash sett -> line smpl actvt=
        for(int i = 0, imax = listK.size(); i < imax; i++){
            if(proflineSmpl == WaterMeterHelper::oneProfile2lineSmpl(profileName2schedule.value(listK.at(i)).toHash())){
                profileName = listK.at(i);
            }


        }
    }


    const QString cellText = Protocol5toGUI::makeSleepProfileLine4waterMeters(profileName, proflineSmpl);





    const int insertIndx = lastNIs.indexOf(ni);
    if(insertIndx >= 0){

        if(cellText == TableViewHelper::getCellValueOfcurrentRow(insertIndx, 1, model))
            return ;//nothing to change

        if(QMessageBox::question(this, windowTitle(), tr("The meter with %1 '%2' is already exists.<br>Do you want to replace?")
                                 .arg(tr("NI")).arg(ni)
                                 , QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) == QMessageBox::No){
            return ;
        }

        model->removeRow(insertIndx);
    }

    QStringList list;
    list.append(QDateTime::currentDateTime().toString(lastDateTimeMask));
    list.append(cellText);
    list.append(QString("%1, %2").arg(gHelper->waterMeter.hashMeterNi2info.value(ni).minfo.model).arg(gHelper->waterMeter.hashMeterNi2info.value(ni).minfo.vrsn));

    list.append(gHelper->waterMeter.hashMeterNi2info.value(ni).sn);
    list.append(ni);
    list.append(gHelper->waterMeter.hashMeterNi2info.value(ni).minfo.memo);
    list.append(gHelper->waterMeter.hashMeterNi2info.value(ni).minfo.crdnts);
    list.append("1");

    StandardItemModelHelper::append2model(list, model);



    QVariantList lh = gHelper->hashUndoHistoryTv.value(ui->tvTable);
    for(int i = 0, iMax = lh.size(); i < iMax; i++){
        const QStringList sl = lh.at(i).toStringList();
        if(sl.size() < 4)
            continue;

        if(sl.at(4) == ni){
            lh.removeAt(i);
            i--;
            iMax--;
        }
    }
    gHelper->hashUndoHistoryTv.insert(ui->tvTable, lh);
    gHelper->resizeTv2content(ui->tvTable);
    showLblMessagePressSaveButton();

    return ;
}


//------------------------------------------------------------------------



void WaterSleepScheduler::on_tvTable_doubleClicked(const QModelIndex &index)
{
    const QString ni = TableViewHelper::getCellValueOfindex(ui->tvTable, index, 4);
    const QString cellTxt = TableViewHelper::getCellValueOfindex(ui->tvTable, index, 1);
//    const QString ni = TableViewHelper::getCellValueOfindex(ui->tvTable, index, 4);

    emit setEditNiPageSett(ni, Protocol5toGUI::getProfLineNameTableCell(cellTxt), Protocol5toGUI::getProfLineFromTableCell(cellTxt));
}

void WaterSleepScheduler::deleteSelected()
{
    gHelper->deleteSelectedFromTable(ui->tvTable);
}

//------------------------------------------------------------------------
