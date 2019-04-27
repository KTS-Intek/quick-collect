#include "pollwdgtmom.h"
#include "src/nongui/settloader.h"
#include "zbyrator-src/settloader4matildaemulator.h"
#include "src/matilda/settloader4matilda.h"

///[!] matilda-bbb-serverside-shared
#include "matilda-bbb-src/decoders/decodematildaprotocolv1.h"

PollWdgtMom::PollWdgtMom(PollWdgt *pollWdgt, QObject *parent) : QObject(parent), pollWdgt(pollWdgt)
{
    connect(pollWdgt, SIGNAL(onPageCanReceiveData()), this, SLOT(onReloadPageSettings()));
    connect(pollWdgt, SIGNAL(onSettingsChanged()), this, SLOT(onSavePageSettings()));

    connect(this, SIGNAL(destroyed(QObject*)), this, SLOT(saveAllForced()));
    ignoreSave = true;
}

void PollWdgtMom::onReloadPageSettings()
{
    const QVariantHash h = DecodeMatildaProtocolV1::readCOMMAND_READ_POLL_SETT().toHash();// SettLoader::loadSett(SETT_ZBRTR_POLLPAGE).toHash();
    ignoreSave = true;
     pollWdgt->setPageSett( h.isEmpty() ? SettLoader4matildaEmulator::defaultPollWdgtSett() : h );
    QTimer::singleShot(333, this, SLOT(unlockIgnore()));
}

void PollWdgtMom::onSavePageSettings()
{
    if(ignoreSave)
        return;
    const QVariantHash h = DecodeMatildaProtocolV1::readCOMMAND_READ_POLL_SETT().toHash();// SettLoader::loadSett(SETT_ZBRTR_POLLPAGE).toHash();
    bool ok;
    QString mess;
    const QVariantHash hh = pollWdgt->getPageSett(ok, mess, false);
    if(ok && h != hh){
//        SettLoader::saveSett(SETT_ZBRTR_POLLPAGE, hh);

        DecodeMatildaProtocolV1::writeCOMMAND_WRITE_POLL_SETT(hh);
    }

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
