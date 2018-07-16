#include "zbyrifacesett.h"
#include "ui_zbyrifacesett.h"
#include "src/matilda/serialporthelper.h"

#include "zbyrator-src/wdgt/defserialdialog.h"

ZbyrIfaceSett::ZbyrIfaceSett(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    MatildaConfWidget(lDevInfo, gHelper, gSett4all, parent),
  ui(new Ui::ZbyrIfaceSett)
{
    ui->setupUi(this);
}

ZbyrIfaceSett::~ZbyrIfaceSett()
{
    delete ui;
}

void ZbyrIfaceSett::setCbxIndx(QComboBox *cbx, const QString &text, const QString &defValue)
{
    const int indx = cbx->findText(text);
    if(indx < 0){
        const int defIndx = cbx->findText(defValue);
        if(defIndx < 0){
            cbx->setCurrentIndex(cbx->count() - 1);
        }else{
            cbx->setCurrentIndex(defIndx);
        }

    }else{
        cbx->setCurrentIndex(indx);
    }
}

//--------------------------------------------------------------------

QVariantHash ZbyrIfaceSett::getPageSett(bool &ok, QString &mess, const bool &is4copy)
{
    Q_UNUSED(is4copy);
    QVariantHash h;
    h.insert("ifaceMode"    , ui->stackedWidget->currentIndex());
    h.insert("uartManual"   , !ui->cbxAutoUartPort->isChecked());
    h.insert("lastUart"     , ui->cbxSerialPort->currentText() );
    h.insert("baudRate"     , ui->cbxBaudRate->currentText());
    h.insert("uartTimeout"  , ui->sbUartTimeout->value());
    h.insert("uartBlockTimeout", ui->sbUartblockTimeout->value());
    h.insert("tcpTimeout"   , ui->sbTcpTimeout->value());
    h.insert("tcpBlcokTimeout", ui->sbTcpBlockTimeout->value());
    h.insert("m2mTimeout"   , ui->sbM2mTimeout->value());
    h.insert("m2mBlockTimeout", ui->sbM2mBlockTimeout->value());
    h.insert("unknownProtocolAsData", ui->cbxUnknownPrtcl->isChecked());
    h.insert("recoverNI"    , ui->cbxRecoverNI->isChecked());

    mess.clear();
    ok = true;

    return h;

}

//--------------------------------------------------------------------

void ZbyrIfaceSett::initPage()
{
    ui->cbxBaudRate->clear();
    ui->cbxBaudRate->addItems(SerialPortHelper::supportedUartSpeedSL());

    on_tbUpdateSerial_clicked();

    ui->rbSerialPort->setChecked(true);
    ui->stackedWidget->setCurrentIndex(0);



    connect(ui->rbSerialPort    , SIGNAL(clicked(bool)), this, SLOT(onRbClicked()) );
    connect(ui->rbTcpIp         , SIGNAL(clicked(bool)), this, SLOT(onRbClicked()) );
    connect(ui->rbM2mService    , SIGNAL(clicked(bool)), this, SLOT(onRbClicked()) );
    onRbClicked();


    emit sendMeIfaceSett();
}

//--------------------------------------------------------------------

void ZbyrIfaceSett::setPageSett(QVariantHash h)
{
//


    switch(h.value("ifaceMode").toInt()){
    case 1 : ui->rbTcpIp->setChecked(true)      ; break;
    case 2 : ui->rbM2mService->setChecked(true) ; break;
    default: ui->rbSerialPort->setChecked(true) ; break;
    }


    ui->cbxAutoUartPort->setChecked(!h.value("uartManual").toBool());

    setCbxIndx(ui->cbxSerialPort, h.value("lastUart").toString(), "rrr");
    setCbxIndx(ui->cbxBaudRate, h.value("baudRate").toString(), "9600");

    ui->sbUartTimeout->setValue(h.value("uartTimeout", 11000).toInt());
    ui->sbUartblockTimeout->setValue(h.value("uartBlockTimeout", 300).toInt());

    ui->sbTcpTimeout->setValue(h.value("tcpTimeout", 11000).toInt());
    ui->sbTcpBlockTimeout->setValue(h.value("tcpBlcokTimeout", 300).toInt());

    ui->sbM2mTimeout->setValue(h.value("m2mTimeout", 11000).toInt());
    ui->sbM2mBlockTimeout->setValue(h.value("m2mBlockTimeout", 300).toInt());


    ui->cbxUnknownPrtcl->setChecked(h.value("unknownProtocolAsData").toBool());
    ui->cbxRecoverNI->setChecked(h.value("recoverNI").toBool());

    on_cbxAutoUartPort_clicked(ui->cbxAutoUartPort->isChecked());
    onRbClicked();

}

//--------------------------------------------------------------------

void ZbyrIfaceSett::on_tbUpdateSerial_clicked()
{
    ui->cbxSerialPort->clear();
    lastUartList = SerialPortHelper::uartList();
    ui->cbxSerialPort->addItems(lastUartList);
}

//--------------------------------------------------------------------

void ZbyrIfaceSett::onRbClicked()
{
    int indx = 0;
    if(ui->rbTcpIp->isChecked())
        indx++;
    if(ui->rbM2mService->isChecked())
        indx += 2;

    ui->stackedWidget->setCurrentIndex(indx);

}

//--------------------------------------------------------------------

void ZbyrIfaceSett::on_pbRestore_clicked()
{
    emit sendMeIfaceSett();
}
//--------------------------------------------------------------------
void ZbyrIfaceSett::on_pbSave_clicked()
{
    bool ok;
    QString mess;
    const QVariantHash h = getPageSett(ok, mess, false);
    if(!mess.isEmpty())
        gHelper->showMess(mess);

    if(!ok)
        return;

    emit setNewSettings(h);

}
//--------------------------------------------------------------------

void ZbyrIfaceSett::on_cbxAutoUartPort_clicked(bool checked)
{
    ui->cbxSerialPort->setDisabled(checked);
    ui->tbUpdateSerial->setDisabled(checked);

    ui->leDefaultPort->setEnabled(checked);
    ui->tbUpdateDefSerial->setEnabled(checked);
}
//--------------------------------------------------------------------
void ZbyrIfaceSett::on_tbUpdateDefSerial_clicked()
{
    on_tbUpdateSerial_clicked();

    if(lastUartList.isEmpty()){
        gHelper->showMess(tr("Can't find any serial port"));
    }else{
        DefSerialDialog *dialog = new DefSerialDialog(lastUartList, lastUartList, this);
        connect(dialog, SIGNAL(setThisPortAsDefault(QString)), ui->leDefaultPort, SLOT(setText(QString)) );
        dialog->exec();
        dialog->deleteLater();
    }
}
