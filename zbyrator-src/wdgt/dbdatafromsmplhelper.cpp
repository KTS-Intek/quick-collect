#include "dbdatafromsmplhelper.h"
#include "dataconcetrator-pgs/dbdataformdefy.h"
#include "src/widgets/tableheaders.h"
#include "src/matilda/showmesshelper.h"
#include "src/meter/meterstatehelper.h"
#include "src/meter/definedpollcodes.h"
#include <QDateTime>

DbDataFromSmplHelper::DbDataFromSmplHelper(QObject *parent) : QObject(parent)
{

}

DbHeaderSett DbDataFromSmplHelper::showHideCb4column4Db(const int &mode, const QStringList &lHeaderData)
{
    //old
    //0 - active -reactive
    //1 - active reactive tariff
    //2 - voltage
    //3 - hide all
    //4 - Write meter Addr log
    //sn and ni always visible
    //    ui->cbSerialNumb4Db->setVisible(mode != 3);
    //    ui->cbNi4Db->setVisible(mode != 3);
    //    ui->cbMemo4Db->setVisible(mode != 3);

    //new
    //1 - active reactive tariff
    //2 - active -reactive
    //3 - voltage
    //4 - meter journal
    //5 - Svaha backups

    QStringList listCol, listData;

    listCol.append(tr("Serial Number;Meter Address;Memo").split(";"));
    listData.append(QString("meter_sn meter_ni memo").split(" "));


//#define DB_SHOW_MODE_ACTV_REACTV_TRFF_ZBRTR 21
//#define DB_SHOW_MODE_ACTV_REACTV_ZBRTR      22
//#define DB_SHOW_MODE_VOLTAGE_ZBRTR          23
//#define DB_SHOW_MODE_METERJOURNL_ZBRTR      24
//#define DB_SHOW_MODE_METER_STATE_ZBRTR      27

    bool checkHeader = true;
    switch(mode){
    case DB_SHOW_MODE_ACTV_REACTV_TRFF_ZBRTR:
    case DB_SHOW_MODE_ACTV_REACTV_TRFF      :{
        listCol.append( QString("A+ A- R+ R- ∑T T1 T2 T3 T4").split(" "));
        listData.append(QString("A+ A- R+ R- T0 T1 T2 T3 T4").split(" "));
        break;}

    case DB_SHOW_MODE_ACTV_REACTV_ZBRTR     :
    case DB_SHOW_MODE_ACTV_REACTV           :{
        listCol.append( QString("A+ A- R+ R-").split(" "));
        listData.append(QString("A+ A- R+ R-").split(" "));
        listCol.append(tr("Interval"));
        listData.append("intrvl");
        break;}

    case DB_SHOW_MODE_VOLTAGE_ZBRTR         :
    case DB_SHOW_MODE_VOLTAGE               :{
        listCol.append( QString("U;I;P;Q;%1").arg(tr("Power factor;Frequency;L1;L2;L3")).split(";"));
        listData.append(QString("U;I;P;Q;cos_f;F;A;B;C").split(";"));
        break;}

    case DB_SHOW_MODE_METERJOURNL_ZBRTR     :
    case DB_SHOW_MODE_METERJOURNL           :{
        listCol.append(      tr("Model;Event Code;Comment"     ).split(";"));
        listData.append(QString("meter_model evnt_code comment").split(" "));
        break;}

    case DB_SHOW_MODE_BACKUPLIST            : listCol = TableHeaders::getColNamesSvahaBackupList().split(",").mid(1); listData = listCol; checkHeader = false; break;
    case DB_SHOW_MODE_PREPAID_REPORT        : listCol = TableHeaders::getColNamespPrepaidReport().split(",").mid(1) ; listData = listCol; checkHeader = false; break;

    case DB_SHOW_MODE_METER_STATE_ZBRTR     :
    case DB_SHOW_MODE_METER_STATE           : listCol = ShowMessHelper::dataKeys2human(MeterStateHelper::getMeterStateHeaderKeys(true).mid(1)); listData = MeterStateHelper::getMeterStateHeaderKeys(true).mid(1);  break;


    case DB_SHOW_MODE_WTR_TARIFS            :
    case DB_SHOW_MODE_WTR_TARIFS_ZBRTR      : listCol = TableHeaders::getColNamesWaterTotalValues().split(","); listData = TableHeaders::getColDataWaterTotalValues().split(","); break;
    }

    switch(mode){

    case DB_SHOW_MODE_WTR_TARIFS_ZBRTR      :

    case DB_SHOW_MODE_ACTV_REACTV_TRFF_ZBRTR:
    case DB_SHOW_MODE_ACTV_REACTV_ZBRTR     :
    case DB_SHOW_MODE_VOLTAGE_ZBRTR         :
    case DB_SHOW_MODE_METERJOURNL_ZBRTR     :
    case DB_SHOW_MODE_METER_STATE_ZBRTR     :{
        listCol.append(tr("Status"));
        listData.append("stts");
        break;}
    }

    if(lHeaderData.contains("intrvl") && !listData.contains("intrvl")){
        listData.append("intrvl");
        listCol.append(tr("Interval"));
    }

    if(checkHeader){
        if(lHeaderData.isEmpty())
            return DbHeaderSett();
        for(int i = 0, iMax = listData.size(), jMax = lHeaderData.size(); i < iMax; i++){
            bool found = false;
            for(int j = 0; j < jMax && !found; j++)
                found = lHeaderData.at(j).contains(listData.at(i));

            if(!found){
                listData.removeAt(i);
                listCol.removeAt(i);
                i--;
                iMax--;
            }
        }
    }
    return DbHeaderSett(listCol, listData);
}

QStringList DbDataFromSmplHelper::checkShowColumn4Db(const int &lastDbFilterMode, const QStringList &lastColumnList4DB, QStringList listEnrg)
{
    QStringList listShow;

    switch(lastDbFilterMode){
    case DB_SHOW_MODE_ACTV_REACTV_TRFF      :
    case DB_SHOW_MODE_ACTV_REACTV_TRFF_ZBRTR:{
        QStringList lignore = QString("msec date_time meter_sn meter_ni memo").split(" ");
        if(lastColumnList4DB.contains("intrvl"))
            lignore.append("intrvl");

        if(lastColumnList4DB.contains("stts"))
            lignore.append("stts");
        //вибираю тарифи
        //∑T    T0 T1 T2 T3 T4

        QStringList tariffList;
        for(int i = 0, iMax = listEnrg.size(); i < iMax; i++){
            if(listEnrg.at(i).length() == 2 && listEnrg.at(i).left(1) == "T"){
                bool ok;
                int t = listEnrg.at(i).right(1).toInt(&ok);
                if(ok && t >= 0 && t < 5){
                    tariffList.append(listEnrg.takeAt(i));
                    i--;
                    iMax--;
                }
            }
        }

        QStringList lSuppEnrg = QString("A+ A- R+ R-").split(" ");
        if(tariffList.isEmpty())
            tariffList.append("0000");//для обробки lignore
        for(int i = 0, eMax = listEnrg.size(), iMax = tariffList.size(); i < iMax; i++){

            for(int e = 0; e < eMax; e++){
                if(lignore.contains(listEnrg.at(e))){
                    if(i == 0)
                        listShow.append(listEnrg.at(e));
                    continue;
                }
                if(lSuppEnrg.contains(listEnrg.at(e)))
                    listShow.append(QString("%1_%2").arg(tariffList.at(i)).arg(listEnrg.at(e)));
            }
        }
        break;}

    case DB_SHOW_MODE_VOLTAGE       :
    case DB_SHOW_MODE_VOLTAGE_ZBRTR :{
        QStringList phsList;
        QStringList lphs = QString("A B C").split(" ");
        for(int i = 0, iMax = listEnrg.size(); i < iMax; i++){
            if(listEnrg.at(i).length() == 1 && lphs.contains(listEnrg.at(i))){
                phsList.append(listEnrg.takeAt(i));
                i--;
                iMax--;
            }
        }
        QStringList lSuppEnrg = QString("U;I;P;Q;cos_f").split(";");
        QStringList lignore = QString("msec date_time meter_sn meter_ni memo F").split(" ");
        if(phsList.isEmpty())
            phsList.append("000");

        if(lastColumnList4DB.contains("stts"))
            lignore.append("stts");

        for(int i = 0, eMax = listEnrg.size(), iMax = phsList.size(); i < iMax; i++){

            for(int e = 0; e < eMax; e++){
                if(lignore.contains(listEnrg.at(e))){
                    if(i == 0)
                        listShow.append(listEnrg.at(e));
                    continue;
                }
                if(lSuppEnrg.contains(listEnrg.at(e)))
                    listShow.append(QString("%1%2").arg(listEnrg.at(e)).arg(phsList.at(i)));
            }
        }
        break;}


    default: listShow = listEnrg; break;
    }


    listShow.prepend("date_time");
    return listShow;
}

int DbDataFromSmplHelper::getLastDbModeFromPollCode(const quint8 &pollCode)
{
    QVariantHash h;
    return getLastDbModeFromPollCode(pollCode, false, h);
}

int DbDataFromSmplHelper::getLastDbModeFromPollCode(const quint8 &pollCode, const bool &isZbyrator, QVariantHash &hash)
{
    int lastDbFilterMode = DB_SHOW_MODE_ACTV_REACTV_TRFF;
//    QStringList listEnrg;

    switch(pollCode){

    case POLL_CODE_READ_TOTAL           :
    case POLL_CODE_READ_END_DAY         :
    case POLL_CODE_READ_END_MONTH       : lastDbFilterMode = DB_SHOW_MODE_ACTV_REACTV_TRFF; break;
    case POLL_CODE_READ_POWER           : lastDbFilterMode = DB_SHOW_MODE_ACTV_REACTV     ; break;

    case POLL_CODE_WTR_METER_STATE      :
    case POLL_CODE_READ_METER_STATE     : lastDbFilterMode = DB_SHOW_MODE_METER_STATE     ; break;// listEnrg = QString("relay,deg,vls,prm").split(","); break;

    case POLL_CODE_WTR_METER_LOGBOOK    :
    case POLL_CODE_READ_METER_LOGBOOK   : lastDbFilterMode = DB_SHOW_MODE_METERJOURNL     ; break;

    case POLL_CODE_WTR_TOTAL            :
    case POLL_CODE_WTR_END_DAY          :
    case POLL_CODE_WTR_END_MONTH        : lastDbFilterMode = DB_SHOW_MODE_WTR_TARIFS      ; break;

    default: lastDbFilterMode = DB_SHOW_MODE_VOLTAGE; break;
    }


    if(isZbyrator){
        lastDbFilterMode += DB_SHOW_DIFF_4_ZBYRATOR;
        switch(pollCode){
        case POLL_CODE_WTR_TOTAL        :
        case POLL_CODE_WTR_METER_STATE  :

        case POLL_CODE_READ_VOLTAGE     :
        case POLL_CODE_READ_TOTAL       :
        case POLL_CODE_READ_METER_STATE : hash.insert("FromDT", QDateTime::currentDateTime()); hash.remove("ToDT"); break;// listEnrg = QString("relay,deg,vls,prm").split(","); break;

        }
    }
    return lastDbFilterMode;
}

bool DbDataFromSmplHelper::getAllowDate2utc(const quint8 &code)
{
    return  (code == POLL_CODE_READ_TOTAL || code == POLL_CODE_READ_VOLTAGE || code == POLL_CODE_READ_POWER || code == POLL_CODE_READ_METER_STATE ||
            code == POLL_CODE_WTR_TOTAL || code == POLL_CODE_WTR_METER_STATE || code == POLL_CODE_WTR_METER_LOGBOOK);
}
