#include "startpagepoll.h"
#include "ui_startpagepoll.h"
#include "src/meter/meterpluginsloadhelper.h"
#include "src/matilda/settloader.h"
//#include "src/matilda/moji_defy.h"
#include "src/meter/definedpollcodes.h"
#include "dataconcetrator-pgs/dbdataform.h"
#include "src/zbyrator-v2/quickpollhelper.h"
#include "src/meter/meterstatehelper.cpp"
#include "zbyrator-src/selectmeters4poll.h"

#include "dataconcetrator-pgs/src/dbdatafromsmplhelper.h"
#include "src/widgets/tableheaders.h"

//---------------------------------------------------------------------

StartPagePoll::StartPagePoll(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    MatildaConfWidget(lDevInfo, gHelper, gSett4all, parent),
    ui(new Ui::StartPagePoll)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------

StartPagePoll::~StartPagePoll()
{
    SettLoader::saveSett(SETT_ZBRTR_POLLPAGE_MODE, ui->tbwTypeOfMeter->currentIndex());
    delete ui;
}

QString StartPagePoll::getArgsElectricMeter(const QDateTime &dtTo, const QDateTime &dtFrom, QString &mess)
{
    return QuickPollHelper::createQuickPollLineSmpl(UC_METER_ELECTRICITY, ui->leOneMeterNI->text(),
                                                                                  (ui->cbxOneMeterModel->currentIndex() > 0) ? ui->cbxOneMeterModel->currentText() : "", ui->leOneMeterPass->text()
                                                    , QString(ui->cbxOneMeterEnergy->currentData().toString()).replace(",", " "), ui->sbOneMeterTariff->value(), dtTo, dtFrom, true, mess);
}
//---------------------------------------------------------------------
QString StartPagePoll::getArgsWaterMeter(const QDateTime &dtTo, const QDateTime &dtFrom, QString &mess)
{
    int secs2sleep = ui->cbxPwrManagement->isChecked() ? ui->sbSleepSecsAfter->value() : 0;

    return QuickPollHelper::createQuickPollLine(UC_METER_WATER, ui->leOneMeterNI_2->text(),
                                         (ui->cbxOneMeterModel_2->currentIndex() > 0) ? ui->cbxOneMeterModel_2->currentText() : "", ""
                             , "", 4, dtTo, dtFrom, true, secs2sleep, ui->cbxCheckSleepProfile->isChecked(), mess);
}

//---------------------------------------------------------------------

void StartPagePoll::setupCbxModel2regExp(QComboBox *cbx, const QVariantHash &hash)
{
    //    QVariantHash hashModels = MeterPluginsLoadHelper::getHashStrPlg();

    cbx->clear();


    const QStringList listModel = hash.value("listModel").toStringList();
    const QStringList listRules = hash.value("listRules").toStringList();

    cbx->addItem("Auto", "^(.){255}$");// "^(a-zA-Z0-9\\s){500}$");
    for(int i = 0, imax = listModel.size(); i < imax; i++)
        cbx->addItem(listModel.at(i), listRules.at(i));


    connect(cbx, SIGNAL(currentIndexChanged(int)), this, SLOT(changeRule2le()) );

    cbx->setCurrentIndex(0);
    QTimer::singleShot(1111, this, SLOT(changeRule2le()));
}

//---------------------------------------------------------------------

QVariant StartPagePoll::getPageSett4read(bool &ok, QString &mess)
{
    Q_UNUSED(ok);
    Q_UNUSED(mess);

    emit killSelectMeters4poll();
    QTimer::singleShot(1, this, SLOT(onPbReadDb_clicked()) );
    return QVariant();
}

//---------------------------------------------------------------------

void StartPagePoll::initPage()
{

    myModelsHlpr = new DbDataFromModelHelper(ui->tbwTypeOfMeter, this);


    ui->swMeterMode->setCurrentIndex(0);
    ui->rbSavedList->setChecked(true);

    setupCbxModel2regExp(ui->cbxOneMeterModel, MeterPluginsLoadHelper::getHashStrPlg(UC_METER_ELECTRICITY));
    setupCbxModel2regExp(ui->cbxOneMeterModel_2, MeterPluginsLoadHelper::getHashStrPlg(UC_METER_WATER));


    dtFromToWdgt = new SelectDtWdgt(this);
    ui->vl4dtWdgt->addWidget(dtFromToWdgt);


    createModelProfile4electricity();
    createModelProfile4water();

    ui->tbwTypeOfMeter->setCurrentIndex(SettLoader::loadSett(SETT_ZBRTR_POLLPAGE_MODE).toInt());

    ui->tabWidget->setCurrentIndex(0);
    ui->tabWidget->currentWidget()->setAccessibleName("main");

    dtFromToWdgt->resetDateAndTime();

    connect(ui->lvMeterDataProfile  , SIGNAL( clicked(QModelIndex)), myModelsHlpr, SLOT(onLvMeterDataProfile_activated(QModelIndex)) );
    connect(ui->lvMeterDataProfile_2, SIGNAL( clicked(QModelIndex)), myModelsHlpr, SLOT(onLvMeterDataProfile_activated(QModelIndex)) );

    myModelsHlpr->onLvInitStep(ui->lvMeterDataProfile->currentIndex(), ui->lvMeterDataProfile_2->currentIndex());



//    connect(ui->lvMeterDataProfile, SIGNAL( clicked(QModelIndex)), this, SLOT(onLvMeterDataProfile_activated(QModelIndex)) );

//    onLvMeterDataProfile_activated(ui->lvMeterDataProfile->currentIndex());

    connect(ui->cbxIgnoreRetr, SIGNAL(clicked(bool)), this, SIGNAL(setIgnoreCycles(bool)));
    QTimer::singleShot(555, this, SIGNAL(onReloadAllMeters()) );

    on_tbwTypeOfMeter_currentChanged(ui->tbwTypeOfMeter->currentIndex());
}
//---------------------------------------------------------------------
void StartPagePoll::setPageSett(const QVariantHash &h)
{
    emit appendData2model(lastWdgtAccssbltName, h);

}

//---------------------------------------------------------------------

void StartPagePoll::onUpdatedSavedList(int activeMetersSize, int switchedOffMetersSize, int meterElectricityActive, int metersWaterActive)
{
    Q_UNUSED(activeMetersSize);
    Q_UNUSED(switchedOffMetersSize);
    ui->rbSavedList->setText(tr("Saved list (electricity - %1, water - %2)").arg(meterElectricityActive).arg(metersWaterActive));
}

//---------------------------------------------------------------------

void StartPagePoll::disconnectMeFromAppendData()
{
    DbDataForm *f = qobject_cast<DbDataForm*>(QObject::sender());
    if(f){
        try{
            f->accessibleName();
        }catch(...){
            return;
        }
        disconnect(this, SIGNAL(appendData2model(QString,QVariantHash)), f, SIGNAL(appendData2model(QString,QVariantHash)) );
        disconnect(f, SIGNAL(disconnectMeFromAppendData()), this, SLOT(disconnectMeFromAppendData()) );
    }
}

//---------------------------------------------------------------------

void StartPagePoll::command4devSlot(quint16 command, QVariantMap map)
{
    createTab(lastSelsett);
    emit command4dev(command, map);
    SettLoader::saveSett(SETT_ZBRTR_POLLPAGE_MODE, ui->tbwTypeOfMeter->currentIndex());

}

//---------------------------------------------------------------------

void StartPagePoll::onPbReadDb_clicked()
{
    emit killSelectMeters4poll();
    emit setIgnoreCycles(ui->cbxIgnoreRetr->isChecked());
    gHelper->updateSettDateMaskAndDotPos();


    lastSelsett = myModelsHlpr->getPollTabSett();

    QString mess;
    metersListMedium->setLastPageId(accessibleName());
    if(ui->rbOneMeter->isChecked()){
        //start poll directly

        if(startPollOneMeterMode(lastSelsett, mess)){
            //create tab
        }else{
            gHelper->showMess(mess);
        }
        return;
    }

    if(startPollAllMetersMode(lastSelsett.code, mess)){

    }else{
        gHelper->showMess(mess);
    }




}

//---------------------------------------------------------------------


void StartPagePoll::on_rbSavedList_clicked()
{
    ui->swMeterMode->setCurrentIndex(0);
}

//---------------------------------------------------------------------

void StartPagePoll::on_rbOneMeter_clicked()
{
    on_tbwTypeOfMeter_currentChanged(ui->tbwTypeOfMeter->currentIndex());
}

//---------------------------------------------------------------------

void StartPagePoll::changeRule2le()
{

    QComboBox *cbx = (ui->swMeterMode->currentIndex() == 2) ? ui->cbxOneMeterModel_2 : ui->cbxOneMeterModel;
    QLineEdit *le = (ui->swMeterMode->currentIndex() == 2) ? ui->leOneMeterNI_2 : ui->leOneMeterNI;
    int indx = cbx->currentIndex();
    QString niReg, passwdReg;

    if(indx >= 0){

        QString regStr = cbx->itemData(indx).toString();
        if(regStr.split("$^").length() == 2 && regStr.left(1) == "^" && regStr.right(1) == "$"){
            niReg = regStr.split("$^").first() + "$";
            passwdReg = "^" + regStr.split("$^").last();
        }else{
            niReg = passwdReg = regStr;
        }

    }

    if(niReg.isEmpty())
        niReg = "^(.){32}$";//Any value

    if(passwdReg.isEmpty())
        passwdReg = "^(.){32}$";


    QRegExp regNI(niReg);
    QRegExpValidator *validatorNI = new QRegExpValidator(regNI, this);

    QString str = le->text();
    int iii = -1;

    while(validatorNI->validate(str, iii) == QRegExpValidator::Invalid)
        str.chop(1);


    le->setValidator(validatorNI);
    le->setText(str);

    if(ui->swMeterMode->currentIndex() > 1)
        return;

    QRegExp regPasswd(passwdReg);
    QRegExpValidator *validatorPasswd = new QRegExpValidator(regPasswd, this);

    iii = -1;
    str = ui->leOneMeterPass->text();
    while(validatorPasswd->validate(str, iii) == QRegExpValidator::Invalid)
        str.chop(1);


    ui->leOneMeterPass->setValidator(validatorPasswd);
    ui->leOneMeterPass->setText(str);
}

//---------------------------------------------------------------------

void StartPagePoll::updatePhValSett()
{
    QVariantMap mpv = SettLoader::loadSett(SETT_METER_MPV).toMap();
    if(mpv.isEmpty())
        mpv = SettLoader::getDefMpv();


    QString currTxt , currData;
    if(!ui->cbxOneMeterEnergy->currentText().isEmpty()){
        currData = ui->cbxOneMeterEnergy->currentData().toString();
        currTxt = ui->cbxOneMeterEnergy->currentText();
    }

    QList<QString> listK = mpv.keys();
    std::sort(listK.begin(), listK.end());

    ui->cbxOneMeterEnergy->clear();
    int indxByTxt = -1, indxByData = -1;
    for(int i = 0, iMax = listK.size(); i < iMax; i++){
        ui->cbxOneMeterEnergy->addItem(listK.at(i), mpv.value(listK.at(i)));
        ui->cbxOneMeterEnergy->setItemData(i, mpv.value(listK.at(i)) , Qt::ToolTipRole );

        if(indxByTxt < 0 && !currTxt.isEmpty() && listK.at(i) == currTxt)
            indxByTxt = i;
        if(indxByData < 0 && !currData.isEmpty() && mpv.value(listK.at(i)) == currData)
            indxByData = i;
    }

    if(indxByTxt >= 0)
        ui->cbxOneMeterEnergy->setCurrentIndex(indxByTxt);
    else if(indxByData >= 0)
        ui->cbxOneMeterEnergy->setCurrentIndex(indxByData);

    if(ui->cbxOneMeterEnergy->count() > 0)
        ui->cbxOneMeterEnergy->setCurrentIndex(0);
}

//---------------------------------------------------------------------

void StartPagePoll::on_swMeterMode_currentChanged(int arg1)
{
    if(arg1 == 1)
        QTimer::singleShot(11, this, SLOT(updatePhValSett()) );

}
//---------------------------------------------------------------------
void StartPagePoll::onLvMeterDataProfile_activated(const QModelIndex &index)
{
    Q_UNUSED(index);
//    if(gHelper->managerEnDisBttn.pbReadDis)
//        return;


//    const quint8 pollCode = modelProfile4electricity->itemData(index).value(Qt::UserRole + 1).toInt();

//    if(pollCode == POLL_CODE_READ_TOTAL || pollCode == POLL_CODE_READ_METER_STATE || pollCode == POLL_CODE_READ_VOLTAGE){
//        ui->pbReadDb->setEnabled(!dtFromToWdgt->isTimeIntervalMode());
//    }else{
//        ui->pbReadDb->setEnabled(true);
//    }

}
//---------------------------------------------------------------------
bool StartPagePoll::startPollOneMeterMode(const StartPollTabSett &selsett, QString &mess)
{
    //QString QuickPollHelper::createQuickPollLine(const QString &line, const QString &cbxText, const QString &passwd, const QString &enrgTxt, const int &tariff, const QDateTime &dtFrom, const int &depth, QString &mess)

    QDateTime dtFrom, dtTo;
    bool isFromMode;
    if(!dtFromToWdgt->getDtVal(dtFrom, dtTo, isFromMode, true)){
        mess = dtFromToWdgt->getLastErr();
        return false;
    }

    if(!dtTo.isValid())
        dtTo = QDateTime::currentDateTime();

    const bool isElectricity = (ui->swMeterMode->currentIndex() == 1);
//const quint8 &meterType, const QString &line, const QString &cbxText, const QString &passwd, const QString &enrgTxt, const int &tariff, const QDateTime &dtTo, const QDateTime &dtFrom, const bool &ignoreExistingData, const int &go2sleepSecs, const QString &sleepProfileLine, QString &mess
    const QString args = isElectricity ? getArgsElectricMeter(dtTo, dtFrom, mess) :
                                         getArgsWaterMeter(dtTo, dtFrom, mess);

    if(args.isEmpty())
        return false;

    createTab(selsett);
    emit command4dev(selsett.code, args);

    return true;

}

bool StartPagePoll::startPollAllMetersMode(const quint8 &pollCode, QString &mess)
{
    QDateTime dtFrom, dtTo;
    bool isFromMode;
    if(!dtFromToWdgt->getDtVal(dtFrom, dtTo, isFromMode, true)){
        mess = dtFromToWdgt->getLastErr();
        return false;
    }


    SelectMeters4poll *w = new SelectMeters4poll(lDevInfo, gHelper, gSett4all, this);
    connect(this, SIGNAL(killSelectMeters4poll()), w, SLOT(deleteLater()) );
    connect(w, SIGNAL(onReloadAllMeters()), this, SIGNAL(onReloadAllMeters()) );
    connect(w, SIGNAL(command4dev(quint16,QVariantMap)), this, SLOT(command4devSlot(quint16,QVariantMap)));
    connect(this, SIGNAL(command4dev(quint16,QString)), w, SLOT(deleteLater()) );
    connect(this, SIGNAL(command4dev(quint16,QVariantMap)), w, SLOT(deleteLater()) );

    connect(metersListMedium, SIGNAL(onAllMeters(UniversalMeterSettList)), w, SIGNAL(onAllMeters(UniversalMeterSettList)) );

    switch(lastSelsett.meterType){
    case UC_METER_ELECTRICITY: w->setPollSettElectric(dtFrom, dtTo, pollCode); break;
    case UC_METER_WATER: w->setPollSettWater(dtFrom, dtTo, pollCode, ui->cbxPwrManagement->isChecked(), ui->sbSleepSecsAfter->value(), ui->cbxCheckSleepProfile->isChecked());

    default: qDebug() << "can't set pollSett StartPagePoll lastSelsett.meterType=" << lastSelsett.meterType;
    }
//    w->setPollSett(dtFrom, dtTo, pollCode, lastSelsett.meterType, (lastSelsett.meterType == UC_METER_WATER && ui->cbxPwrManagement->isChecked()) ? ui->sbSleepSecsAfter->value() : 0);

    emit gHelper->addWdgt2stackWdgt(w, WDGT_TYPE_ZBYR_SELECT_METERS4POLL, false, tr("Select"), ":/katynko/svg/dialog-ok-apply.svg");
    return true;


}
//---------------------------------------------------------------------
void StartPagePoll::createTab(const StartPollTabSett &selsett)
{
    gHelper->updateSettDateMaskAndDotPos();
        gSett4all->updateTableSett();
    QVariantHash hash;

    quint8 code = selsett.code;
    QString txt = selsett.txt;
    QString icon = selsett.icon;

    hash.insert("code", code);

    const bool allowDate2utc = DbDataFromSmplHelper::getAllowDate2utc(code);

    dtFromToWdgt->insert2hashDtFromTo(hash, allowDate2utc);

    const int lastDbFilterMode = DbDataFromSmplHelper::getLastDbModeFromPollCode(code, true, hash);



    DbDataForm *f = new DbDataForm(lDevInfo,gHelper,gSett4all,this);
    f->setSelectSett(hash.value("FromDT").toDateTime(), hash.value("ToDT").toDateTime(), hash.value("ToDT").toDateTime().isValid(), txt, code);
    f->setAccessibleName(QString::number(QDateTime::currentMSecsSinceEpoch()));

    lastWdgtAccssbltName = f->accessibleName();

    connect(f, SIGNAL(disconnectMeFromAppendData()), this, SLOT(disconnectMeFromAppendData()) );

    if(code == POLL_CODE_READ_METER_LOGBOOK || code == POLL_CODE_WTR_METER_LOGBOOK)
        connect(this, SIGNAL(appendData2model(QString,QVariantHash)), f, SIGNAL(appendEvData2model(QString,QVariantHash)) );
    else
        connect(this, SIGNAL(appendData2model(QString,QVariantHash)), f, SIGNAL(appendData2model(QString,QVariantHash)) );

    connect(this, SIGNAL(killTabByName(QString))                , f, SLOT(killTabByName(QString))                 );



    txt.append(", ");
    QDateTime dtFrom = hash.value("FromDT").toDateTime().toLocalTime();
    QDateTime toDt = hash.value("ToDT").toDateTime().toLocalTime();
    if(!allowDate2utc){// code == POLL_CODE_READ_END_DAY || code == POLL_CODE_READ_END_MONTH || code == POLL_CODE_WTR_END_DAY || code == POLL_CODE_WTR_END_MONTH){
        dtFrom = dtFrom.toUTC();
        toDt = toDt.toUTC();
    }


    txt.append(tr("From: %1").arg(dtFrom.toString("yyyy-MM-dd hh:mm:ss")));
    if(hash.value("ToDT").toDateTime().isValid())
        txt.append(", " + tr("To: %1").arg(toDt.toString("yyyy-MM-dd hh:mm:ss")));
    else
        txt.append(", " + tr("To: *"));

    f->setPageMode(lastDbFilterMode, txt, allowDate2utc);//update child accebl name


    emit onPollStarted(code,  getEnrgList4code(code), gHelper->dateMask, gHelper->dotPos, allowDate2utc);
    ui->tabWidget->addTab(f, txt.mid(txt.indexOf(">") + 1));
    ui->tabWidget->setCurrentWidget(f);

    ui->tabWidget->setTabIcon(ui->tabWidget->indexOf(f), QIcon(icon));
}
//---------------------------------------------------------------------
QStringList StartPagePoll::getEnrgList4code(const quint8 &code)
{
    bool hasTariffs = false;
    QStringList l;
    switch(code){
    case POLL_CODE_READ_TOTAL       :
    case POLL_CODE_READ_END_DAY     :
    case POLL_CODE_READ_END_MONTH   : hasTariffs = true;
    case POLL_CODE_READ_POWER       : l = QString("intrvl A+ A- R+ R-").split(" ", QString::SkipEmptyParts); break;

    case POLL_CODE_WTR_METER_STATE  :
    case POLL_CODE_READ_METER_STATE : l = MeterStateHelper::getEngrKeys4table(); break;// listEnrg = QString("relay,deg,vls,prm").split(","); break;
    case POLL_CODE_READ_VOLTAGE     : l = QString("UA,UB,UC,IA,IB,IC,PA,PB,PC,QA,QB,QC,cos_fA,cos_fB,cos_fC,F").split(',')  ; break;

    case POLL_CODE_WTR_METER_LOGBOOK:
    case POLL_CODE_READ_METER_LOGBOOK:
    case POLL_CODE_MATILDA_EVNTS    : l = QString("model,evnt_code,comment").split(','); break;


    case POLL_CODE_WTR_TOTAL        :
    case POLL_CODE_WTR_END_DAY      :
    case POLL_CODE_WTR_END_MONTH    : l = TableHeaders::getColDataWaterTotalValues().split(','); break;
    }


    if(!hasTariffs)
        return l;

    QStringList listEnrg;
    for(int i = 0, emax = l.size(); i < 5; i++){
        for(int e = 1; e < emax; e++)//ignore intrvl
            listEnrg.append(QString("T%1_%2").arg(i).arg(l.at(e)));
    }
    return listEnrg;

}

//---------------------------------------------------------------------

void StartPagePoll::createModelProfile4electricity()
{
    myModelsHlpr->modelProfile4electricity = DbDataFromModelHelper::createModelProfile4electricity(false,false, this);
    ui->lvMeterDataProfile->setModel(myModelsHlpr->modelProfile4electricity);
    ui->lvMeterDataProfile->setCurrentIndex(myModelsHlpr->modelProfile4electricity->index(0,0));

}

//---------------------------------------------------------------------

void StartPagePoll::createModelProfile4water()
{
    myModelsHlpr->modelProfile4water = DbDataFromModelHelper::createModelProfile4water(false, this);
    ui->lvMeterDataProfile_2->setModel(myModelsHlpr->modelProfile4water);
    ui->lvMeterDataProfile_2->setCurrentIndex(myModelsHlpr->modelProfile4water->index(0,0));
}

//---------------------------------------------------------------------

void StartPagePoll::on_tabWidget_tabCloseRequested(int index)
{
    if(index < 0)
        return;

    QString name;
    try{
        name = ui->tabWidget->widget(index)->accessibleName();
    }catch(...){
        name = "0";
    }

    if(name == "main")
        return;

    emit killTabByName(name);
    if(name == lastWdgtAccssbltName && gHelper->managerEnDisBttn.pbReadDis)
        gHelper->pbStopAnimateClickSlot();
    ui->tabWidget->removeTab(index);
}

void StartPagePoll::on_tbwTypeOfMeter_currentChanged(int index)
{
    if(index == 1){//water
        ui->swMeterMode->setCurrentIndex(ui->rbOneMeter->isChecked() ? 2 : 0);
        ui->cbxPwrManagement->show();
        ui->sbSleepSecsAfter->show();
        ui->cbxCheckSleepProfile->show();
    }else{
        //electricity
        ui->swMeterMode->setCurrentIndex(ui->rbOneMeter->isChecked() ? 1 : 0);
        ui->cbxPwrManagement->hide();
        ui->sbSleepSecsAfter->hide();
        ui->cbxCheckSleepProfile->hide();
    }
}
