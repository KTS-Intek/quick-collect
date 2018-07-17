#include "addeditmacform.h"
#include "ui_addeditmacform.h"

#include <QtCore>
#include <QShortcut>
#include <QKeySequence>
#include <QMessageBox>
#include <QClipboard>


#include <QJsonDocument>
#include <QJsonObject>

#include "src/matilda/guihelperdefines.h"

//#define CLBRD_DEV_HISTORY   1005 //обробка окремо, бо містить приховані дані


//---------------------------------------------------------------------------------------------------
AddEditMacForm::AddEditMacForm(IfaceSettLoader *connWdgt, const QString &lastLblTxt, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddEditMacForm)
{
    ui->setupUi(this);

    ifaceLoader = connWdgt;
    preapryPage(lastLblTxt);
    connect2ifaceLoader();
    updateSettFromIfaceLoader();

    connect(ui->pushButton_2, SIGNAL(clicked(bool)), this, SLOT(deleteLater()) );

//    QShortcut *shortCut = new QShortcut(QKeySequence::Cancel, this);
    connect(new QShortcut(QKeySequence::Cancel, this), SIGNAL(activated()), this, SLOT(deleteLater()) );
}
//---------------------------------------------------------------------------------------------------
AddEditMacForm::~AddEditMacForm()
{
    delete ui;
}

//---------------------------------------------------------------------------------------------------
void AddEditMacForm::preapryPage(QString lastLblTxt)
{

    QStringList l = lastLblTxt.split("\t");
    if(!lastLblTxt.isEmpty()){
//MAC && Direct       <object id>\t<user>
        lastLblTxt = l.first();
    }

    startProfileId = lastLblTxt;
    ifaceLoader->addThisDev2connPage(IfaceSettLoader::getProfileSett(lastLblTxt));
//    pageSettFromHash(h);

}
//---------------------------------------------------------------------------------------------------
void AddEditMacForm::setThisMacAndObjName(QString mac, QString id)
{
    ui->leObjectMac->setText(mac);
    ui->leObjectName_2->setText(id);
    ui->tabWidget_2->setCurrentIndex(1);
}
//---------------------------------------------------------------------------------------------------
void AddEditMacForm::setThisObjNameIpPort(QString name, QString ip, quint16 port)
{
    ui->leObjectName->setText(name);
    ui->leIp->setText(ip);
    ui->sbPort->setValue(port);
    ui->tabWidget_2->setCurrentIndex(0);
}

void AddEditMacForm::connect2ifaceLoader()
{
    connect(ifaceLoader, SIGNAL(onTheseLoginPasswd(QString,QString)), this, SLOT(onTheseLoginPasswd(QString,QString))) ;

    connect(ifaceLoader, SIGNAL(onThisMode(int))        , ui->tabWidget_2, SLOT(setCurrentIndex(int)));

    connect(ifaceLoader, SIGNAL(onThisObjName(QString)) , ui->leObjectName, SLOT(setText(QString)) );
    connect(ifaceLoader, SIGNAL(onThisIp(QString))      , ui->leIp, SLOT(setText(QString)));
    connect(ifaceLoader, SIGNAL(onThisPort(int))        , ui->sbPort, SLOT(setValue(int)));


    connect(ifaceLoader, SIGNAL(onThisObjName_2(QString)), ui->leObjectName_2, SLOT(setText(QString)) );
    connect(ifaceLoader, SIGNAL(onThisMac(QString))     , ui->leObjectMac, SLOT(setText(QString)) );
    connect(ifaceLoader, SIGNAL(onThisUseMacAddr2conn(bool)), ui->rbUseMac, SLOT(setChecked(bool)));
    connect(ifaceLoader, SIGNAL(onTheseM2mParams(QString,int)), this, SLOT(onTheseM2mParams(QString,int)) );

    ui->cbxUseCustomService->setChecked(false);
    ui->leIp_2->setText("svaha2.ddns.net");
    ui->sbPort_2->setValue(65000);
}

void AddEditMacForm::updateSettFromIfaceLoader()
{
    preapryPage(startProfileId);
}
//---------------------------------------------------------------------------------------------------
void AddEditMacForm::onTheseLoginPasswd(QString login, QString passwd)
{
    ui->leUser->setText(login);
    ui->lePassword->setText(passwd);

}
//---------------------------------------------------------------------------------------------------
void AddEditMacForm::onTheseM2mParams(QString ip, int port)
{
    ui->cbxUseCustomService->setChecked(ip.isEmpty());
    ui->leIp_2->setText(ip);
    ui->sbPort_2->setValue(port);

}
//---------------------------------------------------------------------------------------------------
void AddEditMacForm::on_cbxUseCustomService_toggled(bool checked)
{
    ui->label_78->setEnabled(checked);
    ui->label_79->setEnabled(checked);
    ui->leIp_2->setEnabled(checked);
    ui->sbPort_2->setEnabled(checked);
}
//---------------------------------------------------------------------------------------------------
void AddEditMacForm::on_pushButton_clicked()
{
    QString mess;
    QVariantHash h = pageSett2hash(mess);

    if(mess.isEmpty()){

        QVariantHash ph = IfaceSettLoader::getM2mProfiles();

        if(startProfileId != h.value("objN").toString() && ph.contains(h.value("objN").toString())){
            if(QMessageBox::question(this, tr("M2M profile"), tr("The profile '%1' is already exists.<br>Replace?").arg(h.value("objN").toString())) != QMessageBox::Yes){
                return;
            }
        }

        ph.insert(h.value("objN").toString(), h);
        IfaceSettLoader::saveM2mProfiles(ph);

        QString back;
        if(!h.value("useMac",true).toBool())
            back.append("\t");

        emit setLblTxt(QString("%1\t%2%3").arg(h.value("objN").toString()).arg(h.value("l").toString()).arg(back));
        deleteLater();
    }else{
        emit showMess(tr("Can't save this sett(<br>%1").arg(mess));
    }
//    h.insert("t", 15);
}
//---------------------------------------------------------------------------------------------------

void AddEditMacForm::on_pushButton_4_clicked()
{
    QStringList list;

    QStringList lRow;
    if(ui->tabWidget_2->currentIndex() == 0){
        lRow.append(ui->leObjectName->text().simplified());
        lRow.append(ui->leUser->text().simplified().trimmed());
        const QString ip = ui->leIp->text();

        lRow.append(ip.contains(":") ?
                        QString("[%1]:%2").arg(ip).arg(ui->sbPort->value()) :
                        QString("%1:%2").arg(ip).arg(ui->sbPort->value()));


    }else{
        lRow.append(ui->leObjectName_2->text().simplified());
        lRow.append(ui->leUser->text().simplified().trimmed());
        lRow.append(ui->cbxUseCustomService->isChecked() ? ui->leIp_2->text().simplified().trimmed() : "");

    }

    if(true){
        QVariantList l;
        if(ui->tabWidget_2->currentIndex() == 0)
            l.append(QString("%1\n%2\n%3").arg(ui->leUser->text().simplified().trimmed()).arg(ui->leIp->text().simplified().trimmed()).arg(ui->sbPort->value()));// Логін адреса порт
        else
            l.append(QString("%1\n%2").arg(ui->leUser->text().simplified().trimmed()).arg(ui->leObjectName_2->text().simplified()));//логін  назваOб'єкту || MAC

        QString mess;
        l.append(pageSett2hash(mess));
        if(!mess.isEmpty()){
            emit showMess(tr("Can't copy this sett(<br>%1").arg(mess));
            return;
        }

        QByteArray writeArr;
        if(true){
            QDataStream out(&writeArr,QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_5_6);
            out << (quint64)0 << l;
            out.device()->seek(0);
            quint64 blSize = writeArr.size();
            out << (quint64)(blSize );
            qDebug() << "blSize " << blSize;
        }

        lRow.append(QString(qCompress(writeArr,9).toBase64(QByteArray::OmitTrailingEquals)));
        qDebug() << "lRow last " << lRow.last().size();

    }

    list.append(lRow.join("\t"));

    QClipboard *clipboard = QApplication::clipboard();
    QJsonObject jobj;
    jobj.insert("type", CLBRD_DEV_HISTORY);
    jobj.insert("name", "kts-intek.com.ua/matilda-conf");
    jobj.insert("version", 1);
    jobj.insert("value", list.join("\n"));

    QByteArray writeArr ;
    if(true){
        QJsonDocument jDoc2DST(jobj);
        writeArr = jDoc2DST.toJson(QJsonDocument::Compact);
        writeArr.chop(1);// remove }
    }
    writeArr.append(",\"hsh\":\"" + QCryptographicHash::hash( writeArr + ",\"hsh\":\"0\"}", QCryptographicHash::Keccak_256).toBase64(QByteArray::OmitTrailingEquals) + "\"}" );
    clipboard->setText(QString(writeArr));
}
//---------------------------------------------------------------------------------------------------
void AddEditMacForm::on_pushButton_3_clicked()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    if(mimeData->hasText()){
        QByteArray arr = QVariant(mimeData->text()).toByteArray().simplified().trimmed();
        while(!arr.isEmpty() && arr.right(1) != "}")
            arr.chop(1);

        if(!messHshIsValid( arr))
            return;

        QJsonParseError jErr;
        QVariantHash hash = QJsonDocument::fromJson( arr, &jErr).object().toVariantHash();
        arr.clear();


        int dataMode = hash.value("type").toInt();

        if(dataMode != CLBRD_DEV_HISTORY || hash.value("name").toString() != "kts-intek.com.ua/matilda-conf")
            return;

        QStringList list = hash.value("value").toString().split("\n", QString::SkipEmptyParts);
        hash.clear();

        bool oneSettNot4mac = false;
        for(int i = 0, iMax = list.size(); i < iMax; i++){
            QStringList sl = list.at(i).split("\t");
            if(sl.isEmpty() || sl.last().isEmpty())
                continue;
            QByteArray arr = qUncompress( QByteArray::fromBase64(sl.last().toLocal8Bit()));
            if(arr.isEmpty())
                continue;

            quint64 len, len2 = arr.length();
            QVariantList l;
            QDataStream stream(&arr, QIODevice::ReadOnly);
            stream >> len >> l;

            if(len != len2){
                qDebug() << "len != len2" << len << len2;
            }

            if(l.size() < 2)
                return;


            QString oneLine = l.first().toString();
            QVariantHash connSett = l.last().toHash();




            if(oneLine.isEmpty() || connSett.isEmpty())
                continue;

            if(pageSettFromHash(connSett)){
                oneSettNot4mac = false;
                break;
            }
            oneSettNot4mac = true;
        }

        if(oneSettNot4mac)
            emit showMess(tr("This settings is not for MAC("));
    }
}
//---------------------------------------------------------------------------------------------------
bool AddEditMacForm::pageSettFromHash(const QVariantHash &oneObj)
{
        //objN mac cmMAC a p
    /*
l - login
objN - object name
useMac - Direct Or MAC server
cmMAC - MAC or object ID
t - time out sec
mac - MAC
pas - password
useCustomSvaha
a - IP URL  //if this values is empty use default svaha.ddns.net and svaha2.ddns.net, TCP port 65000
p - tcp port
*/

    if(!oneObj.contains("useMac") && !oneObj.contains("objN"))
        return false;


    ui->tabWidget_2->setCurrentIndex( oneObj.value("useMac", true).toBool() ? 1 : 0);


    ui->leUser->setText(oneObj.value("l").toString());
    ui->lePassword->setText(oneObj.value("pas").toString());

    if(oneObj.value("useMac", true).toBool()){
        ui->leObjectName_2->setText(oneObj.value("objN").toString());
        ui->leObjectMac->setText(oneObj.value("mac").toString());

        ui->rbUseMac->setChecked(oneObj.value("cmMAC", false).toBool());
        ui->rbUseObjID->setChecked(!ui->rbUseMac->isChecked());

        ui->cbxUseCustomService->setChecked(oneObj.value("useCustomSvaha", true).toBool());

        ui->leIp_2->setText(oneObj.value("a", "svaha2.ddns.net").toString());
        ui->sbPort_2->setValue(oneObj.value("p", 65000).toInt());
    }else{
        ui->leObjectName->setText(oneObj.value("objN").toString());
        ui->leIp->setText(oneObj.value("a").toString());
        ui->sbPort->setValue(oneObj.value("p", 9090).toInt());
    }

    return true;
}
//---------------------------------------------------------------------------------------------------
QVariantHash AddEditMacForm::pageSett2hash(QString &lastError)
{
    lastError = "";
    QVariantHash h;


    if(ui->tabWidget_2->currentIndex() == 0){
        h.insert("objN", ui->leObjectName->text().simplified().trimmed());
        h.insert("a", ui->leIp->text().simplified().trimmed());
        h.insert("p", ui->sbPort->value());
        h.insert("useMac", false);
        if(h.value("objN").toString().isEmpty())
            lastError.append(tr("Object Name is empty<br>"));
        if(h.value("a").toString().isEmpty())
            lastError.append(tr("IP or URL is invalid<br>"));

    }else{
        if(ui->rbUseMac->isChecked()){
            if(ui->leObjectMac->text().simplified().trimmed().isEmpty())
                lastError.append(tr("MAC is empty<br>"));
            if(ui->leObjectName_2->text().simplified().trimmed().isEmpty())
                lastError.append(tr("Object Name is empty<br>"));
        }else{
            if(ui->leObjectName_2->text().simplified().trimmed().isEmpty())
                lastError.append(tr("Object ID is empty<br>"));
        }

        if(ui->cbxUseCustomService->isChecked()){
            if(ui->leIp_2->text().simplified().trimmed().isEmpty())
                lastError.append(tr("IP or URL is invalid<br>"));

            if(ui->sbPort_2->value() < 2)
                lastError.append(tr("TCP port not valid<br>"));
        }
        h.insert("cmMAC", ui->rbUseMac->isChecked());
        h.insert("a", ui->leIp_2->text().simplified().trimmed() );
        h.insert("mac", ui->leObjectMac->text().simplified().trimmed());
        h.insert("useMac", true);
        h.insert("p",  ui->sbPort_2->value());
        h.insert("objN", ui->leObjectName_2->text().simplified());

        h.insert("useCustomSvaha", ui->cbxUseCustomService->isChecked());

        if(h.value("a").toString().isEmpty())
            h.insert("a", "svaha.ddns.net");
        if(h.value("p").toInt() < 2)
            h.insert("p", 65000);

    }

    h.insert("t", 15);

    h.insert("l", ui->leUser->text().simplified().trimmed());
    h.insert("pas", ui->lePassword->text().simplified().trimmed());

    return h;
}
//---------------------------------------------------------------------------------------------------
bool AddEditMacForm::messHshIsValid(QByteArray readArr)
{
    QString hshName("hsh");

    int startIndx = readArr.indexOf(QString("\"%1\":").arg(hshName));
    QByteArray hshBase64;
    if(startIndx > 0){
        startIndx += hshName.length() + 4;
        int endIndx = readArr.indexOf("\"", startIndx + 1);
        hshBase64 = readArr.mid(startIndx , endIndx - startIndx);
        readArr = readArr.left(startIndx ) + "0" + readArr.mid(endIndx);

    }
    if(hshBase64.isEmpty())
        return false;

    hshBase64 = QByteArray::fromBase64(hshBase64);

    QByteArray myHash = QCryptographicHash::hash(readArr, QCryptographicHash::Keccak_256);
    return (myHash == hshBase64);

}
//---------------------------------------------------------------------------------------------------

void AddEditMacForm::on_checkBox_clicked(bool checked)
{
    QString s = ui->lePassword->text();
    ui->lePassword->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::PasswordEchoOnEdit);
    if(checked){
        ui->lePassword->clear();
        ui->lePassword->setText(s);
    }
}
//---------------------------------------------------------------------------------------------------
void AddEditMacForm::on_tbScan0_clicked()
{
    emit openScanWdgt(ui->tabWidget_2->currentIndex(), ifaceLoader);
}
//---------------------------------------------------------------------------------------------------
void AddEditMacForm::on_tbScan1_clicked()
{
    emit openScanWdgt(ui->tabWidget_2->currentIndex(), ifaceLoader);
}
//---------------------------------------------------------------------------------------------------
