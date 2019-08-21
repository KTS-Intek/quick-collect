#include "ifacesett4groups.h"


///[!] quick-collect
#include "ifacesett4groupseditor.h"



///[!] widgets-shared
#include "zbyrator-src/zbyrifacesett.h"


///[!] matilda-bbb-settings
#include "src/matilda/settloader4matilda.h"

///[!] guisett-shared
#include "iface-sett/ifacesett2human.h"

#include "ifaceconnectiondefs.h"


///[!] guisett-shared
#include "iface-sett/ifacesettloader.h"
#include "guisett-shared-src/guiifacemediumcore.h"

IfaceSett4groups::IfaceSett4groups(GuiHelper *gHelper, QWidget *parent) : ReferenceWidgetClassGui(gHelper, parent)
{
    getAddPushButton()->hide();
}

QString IfaceSett4groups::getPrettyIfaceSettings(const QVariantHash &h)
{
    if(h.isEmpty())
        return tr("use global");



    const QString line = IfaceSett2human::getIfaceSettName(h);

    QString errstr;
    //для UC користуюсь кешем, для інших беру з пам'яті
    GuiIfaceMediumCore::convertHashSett2varMap(h, errstr);

    if(!errstr.isEmpty()){
        return tr("%1, error '%2'").arg(line).arg(errstr);
    }
    return line;


//    h.insert("ifaceMode"        , ui->stackedWidget->currentIndex() );

//    h.insert("uartManual"       , !ui->cbxAutoUartPort->isChecked() );
//    h.insert("lastUart"         , ui->cbxSerialPort->currentText()  );
//    h.insert("baudRate"         , ui->cbxBaudRate->currentText()    );
//    h.insert("uartTimeout"      , ui->sbUartTimeout->value()        );
//    h.insert("uartBlockTimeout" , ui->sbUartblockTimeout->value()   );
    //    h.insert("defUart"          , ui->leDefaultPort->text());


//    h.insert("tcpTimeout"       , ui->sbTcpTimeout->value()         );
//    h.insert("tcpBlcokTimeout"  , ui->sbTcpBlockTimeout->value()    );
    //    h.insert("tcpHost"          , ui->leHost->text()                );


//    h.insert("m2mTimeout"       , ui->sbM2mTimeout->value()         );
//    h.insert("m2mBlockTimeout"  , ui->sbM2mBlockTimeout->value()    );
    //    h.insert("m2mProfile"       , ui->leM2mProfile->text()          );


//    h.insert("unknownProtocolAsData", ui->cbxUnknownPrtcl->isChecked());
//    h.insert("recoverNI"        , ui->cbxRecoverNI->isChecked());





}

QStringList IfaceSett4groups::getDefaultHeader()
{
    return tr("Input meter NI;Memo;Interface settings").split(";");
}

void IfaceSett4groups::clearPage()
{
    StandardItemModelHelper::setModelHorizontalHeaderItems(model, getDefaultHeader());

}

void IfaceSett4groups::onEditClick(int srcrow)
{


    const QVariantMap map = SettLoader4matilda().loadOneSett(SETT_QC_UNIQ_IFACE_SETT).toMap();

     QVariantHash h;
     h.insert("groupni", model->item(srcrow, 0)->text());
     h.insert("sett", map.value(h.value("groupni").toString()).toHash());

    emit startEditPageSett(h);

     getMiddleLayout()->setEnabled(false);

     getStackedWidget()->setCurrentIndex(1);

}

//void IfaceSett4groups::setPageSett(const MyListStringList &listRows, const QVariantMap &col2data, const QStringList &headerH, const QStringList &header, const bool &hasHeader)
//{

//}

void IfaceSett4groups::initPage()
{
    createModels(SETT_FILTERS_QC_UNIQ_IFACES);
    createEditor();

    clearPage();

    connect(lastTv, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onLastTvCustomContextMenuRequested(QPoint)));// SLOT(ontvTable_customContextMenuRequested(QPoint)));

    connect(gHelper->lDevInfo, &LastDevInfo::onPowerCenterChanged, this, &IfaceSett4groups::updateAllPowerCenters);

//    connect(this, &DatabaseRules4ip::addEditClick, this, &DatabaseRules4ip::onAddEditClicked);
//    checkCreateEditor();
    readDefCommandOnUpdate();



    QTimer::singleShot(11, this, SLOT(updateAllPowerCenters()));
}

void IfaceSett4groups::updateAllPowerCenters()
{

    const QVariantMap map = SettLoader4matilda().loadOneSett(SETT_QC_UNIQ_IFACE_SETT).toMap();
    MyListStringList listRows;
    addThisMeters(listRows, lDevInfo->matildaDev.ELMETER_PWRINS, map);
    addThisMeters(listRows, lDevInfo->matildaDev.WTRMETER_PWRINS, map);

    setPageSett(listRows, QVariantMap(), getDefaultHeader(), getDefaultHeader(), true);
}

void IfaceSett4groups::onEditorReady()
{
    connect(this, &IfaceSett4groups::editClick, this, &IfaceSett4groups::onEditClick);

}

void IfaceSett4groups::setIfaceSettOneGroup2parent(QString groupni, QVariantHash sett)
{
    const int row = StandardItemModelHelper::getRowFromNI(0, groupni, model);
    getMiddleLayout()->setEnabled(true);
    getStackedWidget()->setCurrentIndex(0);

    if(row < 0)
        return;
    QVariantMap map = SettLoader4matilda().loadOneSett(SETT_QC_UNIQ_IFACE_SETT).toMap();
    map.insert(groupni, sett);
    SettLoader4matilda().saveOneSett(SETT_QC_UNIQ_IFACE_SETT, map);

    emit addIfaceSett2history(sett);

    QTimer::singleShot(111, this, SLOT(updateAllPowerCenters()));


}

void IfaceSett4groups::addThisMeters(MyListStringList &listRows, const QVariantList &l, const QVariantMap &connSettNi2sett)
{
    for(int i = 0, imax = l.size(); i < imax; i++){
        const QVariantHash h = l.at(i).toHash();
        QStringList row;
        row.append(h.value("NI").toString());
        row.append(h.value("memo").toString());
        row.append(getPrettyIfaceSettings(connSettNi2sett.value(row.first()).toHash()));
        listRows.append(row);
    }

}

void IfaceSett4groups::createEditor()
{
    IfaceSett4groupsEditor *w = new IfaceSett4groupsEditor(gHelper, this);
    connect(w, SIGNAL(pageEndInit()), this, SLOT(onEditorReady()));

    connect(w, &IfaceSett4groupsEditor::setIfaceSettOneGroup2parent, this, &IfaceSett4groups::setIfaceSettOneGroup2parent);
    connect(this, SIGNAL(startEditPageSett(QVariantHash)), w, SLOT(setPageSett(QVariantHash)));

    connect(w, SIGNAL(sendMeTheTcpHistory(QLineEdit*))      , this, SIGNAL(sendMeTheTcpHistory(QLineEdit*)));
    connect(w, SIGNAL(sendMeTheM2mHistory(QLineEdit*))      , this, SIGNAL(sendMeTheM2mHistory(QLineEdit*)));

    connect(w, SIGNAL(openEditMacProfileWdgt(bool,QLineEdit*)), this, SIGNAL(openEditMacProfileWdgt(bool,QLineEdit*)));

    connect(w, &IfaceSett4groupsEditor::hideEditor, [=]{
        getMiddleLayout()->setEnabled(true);
        getStackedWidget()->setCurrentIndex(0);
    });

    getStackedWidget()->addWidget(w);
    getMiddleLayout()->setEnabled(true);
    getStackedWidget()->setCurrentIndex(0);

}
