#include "waterscheduleeditorpro.h"
#include "ui_waterscheduleeditorpro.h"

#include "src/widgets/selectionchecker.h"
#include "src/matilda/settloaderdefy.h"
#include "zbyrator-water/src/watersleepschedulesaver.h"
#include "zbyrator-water/editwaterprofile.h"
#include "waterprofilewdgt.h"
#include <QMessageBox>
#include "src/matilda/showmesshelper4wdgt.h"
#include <QShortcut>

WaterScheduleEditorPro::WaterScheduleEditorPro(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    ReferenceWidgetClass(lDevInfo, gHelper, gSett4all, parent),
    ui(new Ui::WaterScheduleEditorPro)
{
    ui->setupUi(this);
    setWindowTitle(tr("Schedule"));

}

WaterScheduleEditorPro::~WaterScheduleEditorPro()
{
    delete ui;
}

void WaterScheduleEditorPro::initPage()
{
    setupObjects(ui->tvTable, ui->tbFilter, ui->cbFilterMode, ui->leFilter, SETT_FILTERS_WTR_PROF_PRO);
    lastTvHighlight = TvHighlightSett("Name");

    connectMessSignal();
    ui->pbDel->setEnabled(false);
    clearPage();


    SelectionChecker *s = new SelectionChecker(this);
    s->setWatchTable(ui->tvTable, ui->pbDel);
    connect(ui->leFilter, SIGNAL(textChanged(QString)), s, SIGNAL(startTmrNow()) );
    connect(ui->cbFilterMode, SIGNAL(currentIndexChanged(int)), s, SIGNAL(startTmrNow()) );

//    connect(gHelper, SIGNAL(setPbWriteOnlyRootEnableDisable(bool)), ui->wdgtAdminButtons, SLOT(setDisabled(bool)));


    WaterProfileWdgt *profileWdgt = new WaterProfileWdgt(false, this);
    ui->vl4previewWdgt->addWidget(profileWdgt);
    connect(this, &WaterScheduleEditorPro::setProfile, profileWdgt, &WaterProfileWdgt::setProfile);
    profileWdgt->setMaximumHeight(ui->pbAdd->height() * 4);


    QTimer *t = new QTimer(this);
    t->setInterval(333);
    t->setSingleShot(true);
    connect(this, SIGNAL(startShowCurrProfileTmr()), t, SLOT(start()) );
    connect(ui->tvTable, SIGNAL(clicked(QModelIndex)), t, SLOT(start()) );

    connect(t, SIGNAL(timeout()), this, SLOT(onShowCurProfileTmr()) );


    setupTbShowHide(ui->tbShowHidePreview, profileWdgt, false, true);

    QTimer::singleShot(333, this, SLOT(createEditWdgt()) );
    //    readDefCommandOnUpdate();
}

void WaterScheduleEditorPro::onEditorReady2edit()
{
    updateSleepProfilesSett();
    ui->wdgtAdminButtons->setEnabled(true);// Disabled(gHelper->managerEnDisBttn.pbWriteOnlyRootDis);

    if(!editThisProfileName.isEmpty()){
        if(TableViewHelper::selectRowWithThisCell(lastTv, editThisProfileName, lastTvHighlight.lastHeaderData.indexOf(lastTvHighlight.key))){
            editThisProfileName.clear();
            onTvTable_doubleclicked(lastTv->currentIndex());
        }
    }
    connect(lastTv, &QTableView::doubleClicked, this, &WaterScheduleEditorPro::onTvTable_doubleclicked);


    QShortcut *shortCut = new QShortcut(QKeySequence::Cancel, this);
    connect(shortCut, SIGNAL(activated()), this, SLOT(deleteLater()));
    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(deleteLater()) );

}


void WaterScheduleEditorPro::openAddEditWdgt(const int &row)
{
    QString name;
    QVariantHash h;

    if(row < 0){
        h = WaterSleepScheduleSaver::getDefaultProfile();
    }else{
        h = model->item(row, 1)->data().toHash();
        name = model->item(row, 0)->text();
    }

    emit setProfileEdit(name, h);

}

void WaterScheduleEditorPro::setEditProfileName(QString name)
{
    editThisProfileName = name;
}

void WaterScheduleEditorPro::createEditWdgt()
{
    EditWaterProfile *editWdgt = new EditWaterProfile(this);
    connect(this, &WaterScheduleEditorPro::setProfileEdit, editWdgt, &EditWaterProfile::setProfileSlot);
    connect(editWdgt, &EditWaterProfile::onSaveProfile, this, &WaterScheduleEditorPro::onSaveProfile);
    connect(editWdgt, SIGNAL(deleteProfileName()), this, SLOT(deleteProfileName()) );
    connect(editWdgt, SIGNAL(ready2edit()), this, SLOT(onEditorReady2edit()) );

    connect(gHelper, SIGNAL(closeYourPopups()), editWdgt, SLOT(smartHide()) );
    connect(this, SIGNAL(onPageActivated()), editWdgt, SLOT(smartShow()) );

    QTimer::singleShot(11, editWdgt, SLOT(setupPage()) );

    connect(this, SIGNAL(onPageActivated()), this, SLOT(updateSleepProfilesSett()) );

}

void WaterScheduleEditorPro::on_pbAdd_clicked()
{
    openAddEditWdgt(-1);
}

void WaterScheduleEditorPro::on_pbDel_clicked()
{
    const int row = getModelCurrentRow();
    if(row < 0)
        return;

    profile2delete = model->item(row, 0)->text();
    deleteProfileName();
}

void WaterScheduleEditorPro::onShowCurProfileTmr()
{
    //to waterprofilewdgt
    const int row = getModelCurrentRow();
    if(row >= 0)
        emit setProfile(model->item(row, 1)->data().toHash());// ui->cbxProfile->currentData().toHash());
}

void WaterScheduleEditorPro::onTvTable_doubleclicked(const QModelIndex &index)
{
    openAddEditWdgt(getModelRow(index));
}

void WaterScheduleEditorPro::onSaveProfile(QString name, QVariantHash profile)
{
    WaterSleepScheduleSaver::addNewProfile(name, profile);
    updateSleepProfilesSett();
    emit updateProfileSett();
}

void WaterScheduleEditorPro::updateSleepProfilesSett()
{
    const QVariantHash h = WaterSleepScheduleSaver::getSavedSett();
    QList<QString> lk = h.keys();
    std::sort(lk.begin(), lk.end());

    //    setPageSett();
    MyListStringList listRows;
    QVariantMap col2data;
    QStringList headerH;
    QStringList header;

    for(int i = 0, imax = lk.size(); i < imax; i++){
        QStringList listonerow;
        listonerow.append(lk.at(i));
        const QVariantHash hash = h.value(lk.at(i)).toHash();
        listonerow.append(QString(QJsonDocument(QJsonObject::fromVariantHash(hash)).toJson(QJsonDocument::Compact)));
        listRows.append(listonerow);
        col2data.insert(QString("%1;1").arg(i), hash);

    }

    header = tr("Name;Schedule").split(";");
    headerH = QString("Name;Schedule").split(";");
    setPageSett(listRows, col2data, headerH, header, true);
}

void WaterScheduleEditorPro::deleteProfileName()
{

    if(profile2delete.isEmpty())
        return;

    if(QMessageBox::question(this, tr("Delete"), tr("Do you want to delete the profile '%1'?").arg(profile2delete)) != QMessageBox::Yes)
        return;


    WaterSleepScheduleSaver::removeOneProfile(profile2delete);
    profile2delete.clear();
    updateSleepProfilesSett();
    emit updateProfileSett();
}



void WaterScheduleEditorPro::on_tvTable_customContextMenuRequested(const QPoint &pos)
{
    gHelper->createCustomMenu(pos, ui->tvTable, (GuiHelper::ShowReset|GuiHelper::ShowExport|GuiHelper::ShowOnlyCopy), CLBRD_SMPL_PRXTBL, ShowMessHelper4wdgt::matildaFileName(windowTitle()));

}
