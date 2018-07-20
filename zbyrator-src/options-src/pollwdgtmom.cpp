#include "pollwdgtmom.h"
#include "src/matilda/settloader.h"
#include "zbyrator-src/settloader4matildaemulator.h"

PollWdgtMom::PollWdgtMom(PollWdgt *pollWdgt, QObject *parent) : QObject(parent), pollWdgt(pollWdgt)
{
    connect(pollWdgt, SIGNAL(onPageCanReceiveData()), this, SLOT(onReloadPageSettings()));
    connect(pollWdgt, SIGNAL(onSettingsChanged()), this, SLOT(onSavePageSettings()));

    ignoreSave = true;
}

void PollWdgtMom::onReloadPageSettings()
{
    const QVariantHash h = SettLoader::loadSett(SETT_ZBRTR_POLLPAGE).toHash();
    ignoreSave = true;
     pollWdgt->setPageSett( h.isEmpty() ? SettLoader4matildaEmulator::defaultPollWdgtSett() : h );
    QTimer::singleShot(333, this, SLOT(unlockIgnore()));
}

void PollWdgtMom::onSavePageSettings()
{
    if(ignoreSave)
        return;
    const QVariantHash h = SettLoader::loadSett(SETT_ZBRTR_POLLPAGE).toHash();
    bool ok;
    QString mess;
    const QVariantHash hh = pollWdgt->getPageSett(ok, mess, false);
    if(ok && h != hh)
        SettLoader::saveSett(SETT_ZBRTR_POLLPAGE, hh);

}

void PollWdgtMom::unlockIgnore()
{
    ignoreSave = false;
}
