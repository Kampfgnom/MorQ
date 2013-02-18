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

bool DownloadController::startNextDownload()
{
    // find the next download
    Download *dl = findNextUnfinishedDownload();
    if(!dl)
        return false;

    foreach(HosterPlugin *hoster, Controller::plugins()->hosterPlugins()) {
        if(hoster->canHandleUrl(dl->url())) {
            Downloader *downloader = hoster->handleDownload(dl);
            m_runningDownloads.insert(dl->id(), downloader);

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

            return true;
        }
    }

    return false;
}

void DownloadController::removeDownload(Download *download)
{
    stopDownload(download);

    Controller::downloadsDao()->remove(download);

    qDebug() << "TEST1";
    if(download->package()) {
        download->package()->removeDownload(download);
    }
    qDebug() << "TEST2";
}

void DownloadController::removePackage(DownloadPackage *package)
{
    foreach(Download *dl, package->downloads()) {
        removeDownload(dl);
    }

    Controller::downloadPackagesDao()->remove(package);
}


void DownloadController::startDownloads()
{
    while(m_runningDownloads.size() < Preferences::maxDownloads()) {
        if(!startNextDownload())
            break;
    }
}

void DownloadController::stopDownload(Download *download)
{
    if(m_runningDownloads.contains(download->id())) {
        m_runningDownloads.value(download->id())->abortDownload();
        m_runningDownloads.remove(download->id());
    }
}
