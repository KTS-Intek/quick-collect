#include "pollwdgtmom.h"
#include "src/nongui/settloader.h"
#include "src/matilda/settloader4matilda.h"

///[!] matilda-bbb-serverside-shared
#include "matilda-bbb-src/decoders/decodematildaprotocolv1.h"


///[!] matilda-bbb-clientside
#include "src/matilda-conf/classmanagerv1.h"

PollWdgtMom::PollWdgtMom(PollWdgt *pollWdgt, QObject *parent) : QObject(parent), pollWdgt(pollWdgt)
{

    connect(pollWdgt->gHelper->ucDeviceTreeW, &UCDeviceTreeWatcher::onPutUCMeterPollSettings, this, &PollWdgtMom::onPutUCMeterPollSettings);
    connect(pollWdgt->gHelper->ucDeviceTreeW, &UCDeviceTreeWatcher::onGetUCMeterPollSettings, this, &PollWdgtMom::onGetUCMeterPollSettings);

//    connect(pollWdgt, SIGNAL(onPageCanReceiveData()), this, SLOT(onReloadPageSettings()));
//    connect(pollWdgt, SIGNAL(onSettingsChanged()), this, SLOT(onSavePageSettings()));

    QTimer *tmrSaveLater = new QTimer(this);
    tmrSaveLater->setInterval(555);
    tmrSaveLater->setSingleShot(true);
    connect(pollWdgt, SIGNAL(onSettingsChanged()), tmrSaveLater, SLOT(start()));
    connect(tmrSaveLater, &QTimer::timeout, this, &PollWdgtMom::onSavePageSettings);

    connect(this, SIGNAL(destroyed(QObject*)), this, SLOT(saveAllForced()));
    ignoreSave = true;
}

void PollWdgtMom::onReloadPageSettings()
{

}

void PollWdgtMom::onSavePageSettings()
{
    if(ignoreSave)
        return;
    QString m;
    pollWdgt->pushPageContent(m); //wait for result at onPutUCMeterPollSettings



}

void PollWdgtMom::unlockIgnore()
{
    ignoreSave = false;
}

void PollWdgtMom::saveAllForced()
{
    unlockIgnore();
    onSavePageSettings();
}

void PollWdgtMom::onGetUCMeterPollSettings(QString senderName)
{
    Q_UNUSED(senderName);

    const QVariantHash h = DecodeMatildaProtocolV1::readCOMMAND_READ_POLL_SETT().toHash();// SettLoader::loadSett(SETT_ZBRTR_POLLPAGE).toHash();
    ignoreSave = true;


    const UCMeterPollSettings settings = ClassManagerV1::fromHash2UCMeterPollSettings(h, pollWdgt->gHelper->ucDeviceTreeW->getProtocolVersion());

    pollWdgt->gHelper->ucDeviceTreeW->setUCMeterPollSettings(settings);
   QTimer::singleShot(333, this, SLOT(unlockIgnore()));
}

void PollWdgtMom::onPutUCMeterPollSettings(UCMeterPollSettings settings, QString senderName)
{
    Q_UNUSED(senderName);


    //old
    const QVariantHash h = ClassManagerV1::fromUCMeterPollSettings2Hash(pollWdgt->gHelper->ucDeviceTreeW->getCachedUCMeterPollSettings());
    //DecodeMatildaProtocolV1::readCOMMAND_READ_POLL_SETT().toHash();// SettLoader::loadSett(SETT_ZBRTR_POLLPAGE).toHash();

//from the widget
    const QVariantHash hh = ClassManagerV1::fromUCMeterPollSettings2Hash(settings);// pollWdgt->getPageSett(ok, mess, false);
    if(h != hh){//something has changed
        //        SettLoader::saveSett(SETT_ZBRTR_POLLPAGE, hh);

        DecodeMatildaProtocolV1::writeCOMMAND_WRITE_POLL_SETT(hh);
    }
}

