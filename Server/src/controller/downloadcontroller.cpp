#include "downloadcontroller.h"

#include "controller.h"
#include "downloader.h"
#include "plugincontroller.h"

#include "plugins/hoster/hosterplugin.h"
#include "preferences.h"

#include <download.h>
#include <downloadpackage.h>

DownloadController::DownloadController(QObject *parent) :
    QObject(parent)
{
}

Download *DownloadController::findNextUnfinishedDownload()
{
    QList<DownloadPackage *> packages = Controller::downloadPackagesDao()->readAll();

    foreach(DownloadPackage *package, packages) {
        if(package->isFinished())
            continue;

        foreach(Download *dl , package->downloads()) {
            if(!dl->isFinished()
                    && !m_runningDownloads.contains(dl->id())
                    && dl->isEnabled())
                return dl;
        }
    }

    return nullptr;
}

void DownloadController::startNextDownload()
{
    // find the next download
    Download *dl = findNextUnfinishedDownload();
    if(!dl)
        return;

    foreach(HosterPlugin *hoster, Controller::plugins()->hosterPlugins()) {
        if(hoster->canHandleUrl(dl->url())) {
            m_runningDownloads.insert(dl->id(), true);
            hoster->handleDownload(dl);

            connect(dl, &Download::finished, [=]() {
                m_runningDownloads.remove(dl->id());
                startNextDownload();
            });

            connect(dl, &Download::enabled, [=]() {
                if(!dl->isEnabled()) {
                    m_runningDownloads.remove(dl->id());
                    startNextDownload();
                }
            });

            return;
        }
    }
}


void DownloadController::startDownloads()
{
    while(m_runningDownloads.size() < Preferences::maxDownloads()) {
        startNextDownload();
    }
}
