#include "ucemulator.h"
#include "ui_ucemulator.h"


///[!] widgets-shared
#include "gui-src/treemodel.h"
#include "gui-src/stackwidgethelper.h"


///[!] quick-collect-gui-core
#include "quick-collect-gui-core/emulator/emulatordevicetree.h"


///[!] matilda-bbb-settings
#include "src/matilda/settloader4matilda.h"


///[!] matilda-bbb-serverside-shared
#include "matilda-bbb-src/decoders/matildaprotocolcore.h"


#include "moji_defy.h"
#include "matildausertypes.h"

UcEmulator::UcEmulator(GuiSett4all *gSett4all, QWidget *parent) :
    StartDevWdgt(true, gSett4all,  parent)

{

    activatePageLater();
}

UcEmulator::~UcEmulator()
{

}

QStringList UcEmulator::realPageNameByDev(const int &devType, const int &protocolVersion)
{
    Q_UNUSED(devType);
    return EmulatorDeviceTree::realPageNameDevEmulator4emulator(protocolVersion);
}

int UcEmulator::getDefDevType()
{
    return DEV_POLL_EMULATOR_L2;
}

int UcEmulator::getDefProtocolVersion()
{
    return MATILDA_PROTOCOL_VERSION_V6;
}

void UcEmulator::createDashBoardWidget()
{
    protocolVersion(getDefProtocolVersion());
    dashBoardW = new SmplPteWdgt("Events", true, true, gHelper, false, this);
    dashBoardSa = StackWidgetHelper::addWdgtWithScrollArea(this, dashBoardW, "dashboard");

//    connect(gHelper, SIGNAL(appendShowMess(QString)), dashBoardW, SLOT(appendPteText(QString)));

    connect(this, SIGNAL(killMyChild()), dashBoardW, SLOT(deleteLater()) );

    connect(this, SIGNAL(appendShowMess(QString)), dashBoardW, SLOT(appendHtml(QString)));


    QTimer *t = new QTimer(this);
    t->setSingleShot(true);
    t->setInterval(222);
    connect(this, SIGNAL(startRealodSettLater()), t, SLOT(start()));
    connect(t, SIGNAL(timeout()), this, SIGNAL(reloadSettings2ucEmulator()));

//    connect(dashBoardW, SIGNAL(setLblWaitTxt(QString)), this, SIGNAL(setLblWaitTxt(QString)));
}

QScrollArea *UcEmulator::getDashBoardScrollArea()
{
    return dashBoardSa;

}

MatildaConfWidget *UcEmulator::getDashBoardWidget()
{
    return dashBoardW;

}

void UcEmulator::createClassManager()
{
    createDecoder();

    manager = new ClassManagerProcessor(this);
    manager->initObjects();
    manager->accessLevel = MTD_USER_ADMIN;


    connect(manager, SIGNAL(onCOMMAND2GUI(quint16,MyListStringList,QVariantMap,QStringList,QStringList,bool)), this, SLOT(onCOMMAND2GUI(quint16,MyListStringList,QVariantMap,QStringList,QStringList,bool)));
    connect(manager, SIGNAL(onCOMMAND2GUI(quint16,QString)), this, SLOT(onCOMMAND2GUI(quint16,QString)));
    connect(manager, SIGNAL(onCOMMAND2GUI(quint16,QVariantHash)), this, SLOT(onCOMMAND2GUI(quint16,QVariantHash)));

    connect(manager, &ClassManagerProcessor::onOperationNError, this, &UcEmulator::onOperationNError);


    getPbLogin()->hide();
    getPbLogOut()->hide();


    connect(manager, SIGNAL(showMess(QString)), this, SLOT(onCOMMAND2GUIreadySlot()));


    connect(this, &UcEmulator::data2matildaExt, this, &UcEmulator::data2matildaExtSlot);


    manager->protocolVersionSlot(getDefProtocolVersion());


//    connect(this, SIGNAL(startDaServer(qint8,quint16))      , manager, SIGNAL(startDaServer(qint8,quint16))   );
//    connect(this, SIGNAL(stopDaServer())                    , manager, SIGNAL(stopDaServer())                 );
//    connect(this, SIGNAL(setDaForwardNI(QByteArray))        , manager, SIGNAL(setDaForwardNI(QByteArray))     );


}

void UcEmulator::onNewMatildaWidgetAdded(MatildaConfWidget *w)
{
    connect(w, &MatildaConfWidget::pageEndInit, [=]{
        connect(w, SIGNAL(onPageActivated()), w, SLOT(checkUpdatePageDefCommand()) );
         w->checkUpdatePageDefCommand();
    });
}

void UcEmulator::activatePageLater()
{
    QTimer::singleShot(111, this, SLOT(activatePage()));
}

void UcEmulator::data2matildaExtSlot(quint16 command, QVariant varData, int secs4loop)
{
    setPbReadDisabled(true) ;
    setPbWriteDisabled(true);

    Q_UNUSED(secs4loop);
    writelater.command.append(command);
    writelater.varData.append(varData);

    QTimer::singleShot(111, this, SLOT(mWriteLater()));
}

void UcEmulator::mWriteIneedMoreTime(const quint16 &command, const qint32 &elTime, const qint64 &someData)
{
    if(socketcache.verboseMode)
        qDebug() << "mWriteIneedMoreTime " << command << elTime << someData;
    decoder->speedStat.buvIneedMoreTime = true;
//    if(decoder->useJsonMode)
//        mWrite2SocketJSON(DecodeMatildaProtocolV1::mWriteIneedMoreTimeJSON(command, elTime, someData), COMMAND_I_NEED_MORE_TIME);
//    else
    mWriteToSocket(DecodeMatildaProtocolV1::mWriteIneedMoreTime(command, elTime, someData), COMMAND_I_NEED_MORE_TIME);
}

void UcEmulator::disconnLater(qint64 msec)
{
    QTimer::singleShot(msec, this, SLOT(onDisconnByDecoder()));
}

void UcEmulator::mWriteToSocket(const QVariant &s_data, const quint16 &s_command)
{
    manager->data2gui(s_command, s_data);
}

void UcEmulator::onDisconnByDecoder()
{
    onDisconnExt(false);
}

void UcEmulator::onDisconnExt(const bool &allowdecoder)
{

    const QDateTime currdt = QDateTime::currentDateTimeUtc();
    if(socketcache.verboseMode && allowdecoder){
        qDebug() << QString("Received: %1, Transmitted: %2, WriteCount: %3, ReadCount: %4, descr: %5").arg(decoder->speedStat.totalBytesRecvd).arg(decoder->speedStat.totalBytesTransm)
                    .arg(decoder->speedStat.writeCounter).arg(decoder->speedStat.readCounter).arg("mySd");
        qDebug() << "conn time UTC/Local/secs " << decoder->speedStat.dtStartUtc.toString("yyyy-MM-dd hh:mm:ss") << decoder->speedStat.dtStartUtc.toLocalTime().toString("yyyy-MM-dd hh:mm:ss")
                 << decoder->speedStat.dtStartUtc.secsTo(currdt);
        qDebug() << "---------------- connection closed -----------------------";


    }
    if(allowdecoder){

        emit addError2Log(QString("Received: %1, Transmitted: %2, WriteCount: %3, ReadCount: %4, descr: %5, ip: %6")
                          .arg(decoder->speedStat.totalBytesRecvd).arg(decoder->speedStat.totalBytesTransm).arg(decoder->speedStat.writeCounter).arg(decoder->speedStat.readCounter).arg("mySd")
                          .arg("myIp"));
        emit addError2Log(QString("conn time UTC: %1, Local: %2, secs: %3, blockDeleteLater: %4, descr: %5, ip: %6")
                          .arg(decoder->speedStat.dtStartUtc.toString("yyyy-MM-dd hh:mm:ss"))
                          .arg(decoder->speedStat.dtStartUtc.toLocalTime().toString("yyyy-MM-dd hh:mm:ss"))
                          .arg(decoder->speedStat.dtStartUtc.secsTo(currdt)).arg(int(0)).arg("mySd").arg("myIp"));
    }
    createDecoderLater();

}

void UcEmulator::onCOMMAND2GUIreadySlot()
{
    setPbReadDisabled(false) ;
    setPbWriteDisabled(false);

}

void UcEmulator::saveSettings(int block, QVariantHash hash)
{
    switch(block){
    case SETT_TCP:{

        SettLoader4matilda sSaver;

        const QList<int> keys = QList<int>() << SETT_TCP_PPPD_FIRST << SETT_TCP_READ_TO << SETT_TCP_READ_TOB << SETT_TCP_MNOZNYK;
        const QStringList l = QString("pppdFirst,tcpRT,tcpRTB,mnznk").split(",");

        qDebug() << "save SETT_TCP sett " << hash;
        for(int i = 0, iMax = l.size(), iMax2 = keys.size(); i < iMax && i < iMax2; i++){
            qDebug() << "save SETT_TCP sett " << hash.contains(l.at(i)) << l.at(i) << keys.at(i) << hash.value(l.at(i));
            if(hash.contains(l.at(i)))
                sSaver.saveOneSett(keys.at(i), hash.value(l.at(i)));

        }

        emit command2extension(MTD_EXT_NAME_ALL, MTD_EXT_COMMAND_RELOAD_SETT, true);//reload peredavator active socket sett

        break;}

    }
}

void UcEmulator::activatePage()
{
    initializeClassManager();

    getStackedWdgt()->setCurrentIndex(1);
    devTypeChanged(getDefDevType(), getDefProtocolVersion());// defaultDevType, defaultProtocolVersion);

    pbManager.pbReadVisible = pbManager.pbWriteVisible = true;

    authrizeAccess(MTD_USER_ADMIN);

    connect(this, SIGNAL(onCOMMAND2GUIready()), this, SLOT(onCOMMAND2GUIreadySlot()));
}

void UcEmulator::createDecoderLater()
{
    QTimer::singleShot(111, this, SLOT(createDecoder()));

}

void UcEmulator::createDecoder()
{
    socketcache = CachedWriteSett(qAppName(), getDefDevType(), true);

    decoder = new DecodeMatildaProtocolWithJSON(this);

    connect(decoder, &DecodeMatildaProtocolWithJSON::addThisIPToBlackList   , this, &UcEmulator::addThisIPToBlackList );
    connect(decoder, &DecodeMatildaProtocolWithJSON::removeThisIpFromBlackList, this, &UcEmulator::removeThisIpFromBlackList);
    connect(decoder, &DecodeMatildaProtocolWithJSON::addError2Log           , this, &UcEmulator::addError2Log         );
    connect(decoder, &DecodeMatildaProtocolWithJSON::setIneedMoreTime       , this, &UcEmulator::setIneedMoreTime     );
    connect(decoder, &DecodeMatildaProtocolWithJSON::command2extension      , this, &UcEmulator::command2extension    );
    connect(decoder, &DecodeMatildaProtocolWithJSON::saveSettings           , this, &UcEmulator::saveSettings         );
    connect(decoder, &DecodeMatildaProtocolWithJSON::addData2prepaidTable   , this, &UcEmulator::addData2prepaidTable );


    //other
    connect(decoder, &DecodeMatildaProtocolWithJSON::command4dev            , this, &UcEmulator::command4dev          );

    connect(decoder, &DecodeMatildaProtocolWithJSON::reloadSErverSett       , this, &UcEmulator::reloadSErverSett     );
    connect(decoder, &DecodeMatildaProtocolWithJSON::reloadMacSett          , this, &UcEmulator::reloadMacSett        );
    connect(decoder, &DecodeMatildaProtocolWithJSON::reloadUdpBeaconSett    , this, &UcEmulator::reloadUdpBeaconSett  );
    connect(decoder, &DecodeMatildaProtocolWithJSON::mWriteIneedMoreTime    , this, &UcEmulator::mWriteIneedMoreTime  );


//    void writeThisDataJSON(QJsonObject data, quint16 command);
//    void sendDirectAccessData(QByteArray arr, qint64 msec, bool addmsec);

//    void daSocketClosed();


//    void writeThisData(QVariant data, quint16 command);

//    void disconnLater(qint64 msec);

//    connect(decoder, &DecodeMatildaProtocolWithJSON::writeThisDataJSON      , this, &UcEmulator::mWrite2SocketJSON);
//    connect(decoder, &DecodeMatildaProtocolWithJSON::sendDirectAccessData, this, &UcEmulator::m)
//    connect(decoder, &DecodeMatildaProtocolWithJSON::daSocketClosed, )
    connect(decoder, &DecodeMatildaProtocolWithJSON::writeThisData          , this, &UcEmulator::mWriteToSocket);
    connect(decoder, &DecodeMatildaProtocolWithJSON::disconnLater           , this, &UcEmulator::disconnLater);


    //from Ucon socket
    ///to signals
    connect(this, &UcEmulator::restartTimeObject                  , decoder, &DecodeMatildaProtocolWithJSON::restartTimeObject);

    //end form UCon socket


    decoder->initObjects();
    decoder->setConnectionSettings(socketcache.enBackupMode, socketcache.devType, socketcache.nameStr, "local", "localid", QString::number(0));
    decoder->updateChasovyjMnoznykPereda4i(socketcache.chasovyjMnoznykPereda4i);



    connect(decoder, SIGNAL(destroyed(QObject*)), this, SLOT(onDisconnByDecoder()));

    QTimer::singleShot(111, this, SLOT(makeAuthorization()));


}

void UcEmulator::makeAuthorization()
{
//    decoder->authorizeF()
    disconnect(manager, SIGNAL(showMess(QString)), gHelper, SLOT(appendShowMessSlot(QString)));
    disconnect(manager, SIGNAL(showMessCritical(QString)), gHelper, SLOT(showMessSlot(QString)));

    decoder->lastObjSett.tmpStamp = "localhost";

    const QVariantHash savedHash = SettLoader4matilda().loadOneSett(SETT_SOME_SETT).toHash();

    const QString leftPartOfKey = "root";
    const QString l = QString("%1_l").arg(leftPartOfKey);
    const QString p = QString("%1_p").arg(leftPartOfKey);
    const QCryptographicHash::Algorithm defhash = MatildaProtocolCore::getDefAlgorithm();


    const QByteArray lh = (savedHash.value(l).toByteArray().isEmpty()) ?  QCryptographicHash::hash("admin", defhash) : savedHash.value(l).toByteArray();
    const QByteArray ph = (savedHash.value(p).toByteArray().isEmpty()) ?  QCryptographicHash::hash(QByteArray(""), defhash) : savedHash.value(p).toByteArray();

    MatildaProtocolCore::calclLoginHash(lh, ph, decoder->lastObjSett.tmpStamp, defhash);




    QVariantHash hash;
    hash.insert("QDS", QString::number(QDataStream::Qt_5_6));
    hash.insert("version", MATILDA_PROTOCOL_VERSION_V6);

    hash.insert("hsh", MatildaProtocolCore::calclLoginHash(lh, ph, decoder->lastObjSett.tmpStamp, defhash).toBase64());//QString(QCryptographicHash::hash(lh + "\n" + decoder->lastObjSett.tmpStamp + "\n" + ph, QCryptographicHash::Keccak_256).toBase64()));

    bool doAfter;
    const FunctionRezultJSON r = decoder->onCOMMAND_AUTHORIZE_JSON(hash, doAfter);

    qDebug() << "rez " << r.s_data << r.s_data;

    qDebug() << "access level " << decoder->accessLevel;

    connect(manager, SIGNAL(showMess(QString)), gHelper, SLOT(appendShowMessSlot(QString)));
    connect(manager, SIGNAL(showMessCritical(QString)), gHelper, SLOT(showMessSlot(QString)));
}

void UcEmulator::mWriteLater()
{
    if(!writelater.command.isEmpty()){
        if(writelater.command.first() > COMMAND_WRITE_FIRST_4_OPERATOR)
            emit startRealodSettLater();

        decoder->decodeReadData(writelater.varData.takeFirst(), writelater.command.takeFirst());
        if(!writelater.command.isEmpty())
            QTimer::singleShot(111, this, SLOT(mWriteLater()));
    }

}

