#include "watersleepschedulermom.h"

///[!] zbyrator-shared
#include "src/zbyrator-v2/quickpollhelper.h"


///[!] matilda-bbb-serverside-shared
#include "matilda-bbb-src/decoders/decodematildaprotocolv5.h"


///[!] zbyrator-shared
#include "src/zbyrator-v2/watermeterhelper.h"

///[!] meters-shared
#include "zbyrator-src/protocol5togui.h"


///[!] matilda-bbb-clientside
#include "src/matilda-conf/classmanagerv5.h"


WaterSleepSchedulerMom::WaterSleepSchedulerMom(GuiHelper *gHelper, QWidget *parent) : WaterSleepScheduler(gHelper, parent)
{
    createTopWidget();
    connect(this, &WaterSleepSchedulerMom::pageEndInit, this ,&WaterSleepSchedulerMom::setupButtonsWidget);

//    connect(this, &WaterSleepSchedulerMom::onSettingsChanged, this, &WaterSleepSchedulerMom::onSettChangedSlot); press save button?
}

QStringList WaterSleepSchedulerMom::getSelectedNIs()
{
    return TableViewHelper::getSelectedRowsText(lastTv, 4);
}

QStringList WaterSleepSchedulerMom::getVisibleNIs()
{
    return TableViewHelper::getRowsText(lastTv, 4);

}

QHash<QString, QStringList> WaterSleepSchedulerMom::getSleepLines2nis(const QStringList &listNi, QStringList &lk)
{
    const UCWMeterLastProfilesSettings settings = getPageSettings();

    QHash<QString, QStringList> out;

    for(int i = 0, imax = listNi.size(); i < imax; i++){
        const QString ni = listNi.at(i);
        const QString sleepLine = settings.lastWaterMeterProfiles.value(ni).sleepProfileLine;

        if(sleepLine.isEmpty()){
            out.clear();
            break;
        }

        QStringList l ;
        if(out.contains(sleepLine))
            l = out.value(sleepLine);
        else
            lk.append(sleepLine);

        l.append(ni);
        out.insert(sleepLine, l);
    }

    return out;

}

void WaterSleepSchedulerMom::onSettChangedSlot()
{
    QStringList messasgel;
    const QVariantList varl = ClassManagerV5::fromUCWMeterLastProfilesSettings(getPageSettings(), messasgel) ;// = Protocol5toGUI::getProfLinesFromTableCell(getPageSett(ok, mess, false));

//    QVariant ClassManagerV5::preparyCOMMAND_WRITE_WMETER_LAST_PROFILES(const QVariant &dataVar)


    IneedMoreTimeObject *timeObject = new IneedMoreTimeObject(this);
    LastMatildaObjSett lastObjSett;
    connect(this, SIGNAL(reloadSavedSleepProfiles()), timeObject, SLOT(deleteLater()));
    DecodeMatildaProtocolV5(timeObject, lastObjSett, this).saveWaterMetersProfilesSmpl(varl);// h.value("lv").toList());

    QTimer::singleShot(111, this, SIGNAL(reloadSavedSleepProfiles()));
}

//void WaterSleepSchedulerMom::waterMeterSchedulerStts(QString ni, QDateTime dtLocal, QString stts, QVariantHash sheduler, QString src)
//{
//    if(ni.isEmpty())
//        return;

//    UCWMeterLastProfileOneRow onerow;
//    onerow.src = src.toUInt();
//    onerow.sleepProfileLine = WaterMeterHelper::oneProfile2lineSmpl(sheduler);
//    onerow.dtmsec = dtLocal.toMSecsSinceEpoch();
//    onerow.sn = ni2sn.value(ni);

//    lastSettings.lastWaterMeterProfiles.insert(ni, onerow);
//    emit startTmrSendCachedData();



//    emit waterMeterSchedulerStts(oneMeter.ni, currDtUtc.toLocalTime(), h.isEmpty() ? tr("no data") : tr("ok"), h, "0");//automatically
//    emit waterMeterSchedulerStts(list.at(0), QDateTime::fromMSecsSinceEpoch(list.at(2).toLongLong()).toLocalTime(), tr("saved"), WaterMeterHelper::oneProfileFromLineSettVarHash(list.at(3)), list.at(4));

//    int row = StandardItemModelHelper::getRowFromNI(4, ni, model);
//    if(row < 0){
//        if(ni.isEmpty())
//            return;

//        QStringList l;
//        const int c = model->columnCount();
//        if(c < 1)
//            return;//there is no data in the model
//        for(int i = 0; i < c; i++)
//            l.append("");
//        l.replace(4, ni);
//        StandardItemModelHelper::append2model(l, model);
//        row = StandardItemModelHelper::getRowFromNI(4, ni, model);
//        if(row < 0)
//            return;
//    }

//    //    stts.append("\n\n");
//    //    const QStringList l = stts.split("\n");

//    if(lastDateTimeMask.isEmpty()){
//        gHelper->updateSettDateMaskAndDotPos();
//        lastDateTimeMask = gHelper->dateMask + " hh:mm:ss";
//    }



//    model->item(row, 0)->setText(dtLocal.toString(lastDateTimeMask));
//    QString profname = stts;
//    if(!sheduler.isEmpty()){
//        const int indx = ui->cbxProfile->findData(sheduler);
//        profname = (indx < 1) ?
//                    "" : // QString(QJsonDocument(QJsonObject::fromVariantHash(sheduler)).toJson(QJsonDocument::Compact)) :
//                    ui->cbxProfile->itemText(indx);

//    }

//    const QString profilelinejson = Protocol5toGUI::makeSleepProfileLine4waterMeters(profname, sheduler);

//    model->item(row, 1)->setText(profilelinejson);// const QString &proflinesmpl));//  stts);

//    model->item(row, 7)->setText(src);

//    //    model->item(row, 1)->setData(sheduler, Qt::UserRole + 1);

//    //    model->item(row, 6)->setText(l.at(1));

//    TableViewHelper::selectRowWithThisCell(ui->tvTable, ni, 4);

//    resizeLastTv2content();
//}

void WaterSleepSchedulerMom::onDoOperationSelected(quint8 operation)
{
    startOperation(getSelectedNIs(), operation);
}

void WaterSleepSchedulerMom::startOperation(const QStringList &listni, const quint8 &pollCode)
{
    if(gHelper->managerEnDisBttn.pbReadDis)
        return;

    if(listni.isEmpty()){
        emit showMessage(tr("no meters"));
        return;
    }

    QList<QVariantMap> map2dev;
    QString messageStrr;

    if(pollCode == POLL_CODE_WRITE_WTR_SLEEP){


        QStringList lk;
        const QHash<QString,QStringList> profile2nis = getSleepLines2nis(listni, lk);

        if(profile2nis.isEmpty()){
            emit showMessage(tr("bad profile"));
            return;
        }


        for(int i = 0, imax = lk.size(); i < imax; i++){
            const QString sleepLine = lk.at(i);

            const QVariantMap map = QuickPollHelper::createPollMapWithParams(profile2nis.value(sleepLine), sleepLine, messageStrr);
            if(!map.isEmpty())
                map2dev.append(map);
        }



    }else{

        const QVariantMap map = QuickPollHelper::createPollMapWithParams(listni, "-", messageStrr);
        if(!map.isEmpty())
            map2dev.append(map);
    }



    if(map2dev.isEmpty())
        return;

    lastTv->clearSelection();
    StandardItemModelHelper::clearCells(listni, 4,QList<int>() << 0 << 1, model);
    gHelper->updateSettDateMaskAndDotPos();
    emit setLastPageId(accessibleName());
//    lastDateTimeMask = gHelper->getDateTimeMask();

    for(int i = 0, imax = map2dev.size(); i < imax; i++)
        emit command4dev(pollCode, map2dev.at(i));

}

void WaterSleepSchedulerMom::setupButtonsWidget()
{
    buttonsWidget->setEnabled(true);
    connect(buttonsWidget, &WaterSleepSchedulerMomButtonsWdgt::onDoOperationSelected, this ,&WaterSleepSchedulerMom::onDoOperationSelected);

    buttonsWidget->createSelectionChecker(lastTv);

    connect(this, &WaterSleepSchedulerMom::onUserChangedTheModel, this, &WaterSleepSchedulerMom::onSettChangedSlot);



}

void WaterSleepSchedulerMom::createTopWidget()
{
    buttonsWidget = new WaterSleepSchedulerMomButtonsWdgt(this);
    getTopLayout()->insertWidget(1, buttonsWidget);
    buttonsWidget->setEnabled(false);


    connect(this, &WaterSleepSchedulerMom::lockButtons, buttonsWidget, &WaterSleepSchedulerMomButtonsWdgt::lockButtons);

}
