#include "downloadcontroller.h"

#include "controller.h"
#include "downloader.h"
#include "plugincontroller.h"

#include "plugins/hoster/hosterplugin.h"
#include "preferences.h"

#include <download.h>
#include <downloadpackage.h>

#include <QDir>

DownloadController::DownloadController(QObject *parent) :
    QObject(parent)
{
}

Download *DownloadController::findNextUnfinishedDownload()
{
    QList<DownloadPackage *> packages = Controller::downloadPackagesDao()->readAll();

    foreach(DownloadPackage *package, packages) {
        if(package->isDownloadFinished())
            continue;

        foreach(Download *dl , package->downloads()) {
            if(!dl->isDownloadFinished()
                    && !m_runningDownloaders.contains(dl->id())
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
    if(!dl) {
        emit statusChanged();
        return false;
    }

    foreach(HosterPlugin *hoster, Controller::plugins()->hosterPlugins()) {
        if(hoster->canHandleUrl(dl->url())) {
            Downloader *downloader = hoster->handleDownload(dl);
            m_runningDownloaders.insert(dl->id(), downloader);
            m_runningDownloads.append(dl);

            connect(dl, &Download::downloadFinished, [=]() {
                m_runningDownloaders.remove(dl->id());
                m_runningDownloads.removeAll(dl);
                startNextDownload();
            });

            connect(dl, &Download::enabled, [=]() {
                if(!dl->isEnabled()) {
                    m_runningDownloads.removeAll(dl);
                    m_runningDownloaders.remove(dl->id());
                    startNextDownload();
                }
            });

            emit statusChanged();
            return true;
        }
    }

    return false;
}

void DownloadController::removeDownload(Download *download)
{
    bool wasRunning = false;

    if(m_runningDownloaders.contains(download->id())) {
        stopDownload(download);
        wasRunning = true;
    }

    Controller::downloadsDao()->remove(download);

    if(download->package()) {
        download->package()->removeDownload(download);
    }

    if(wasRunning)
        startDownloads();
}

void DownloadController::removePackage(DownloadPackage *package)
{
    foreach(Download *dl, package->downloads()) {
        if(dl)
            removeDownload(dl);
    }

    Controller::downloadPackagesDao()->remove(package);
}

void DownloadController::resetDownload(Download *download)
{
    bool wasRunning = false;

    if(m_runningDownloaders.contains(download->id())) {
        stopDownload(download);
        wasRunning = true;
    }

    QFile file(Preferences::downloadFolder() + QDir::separator() + download->fileName());
    if(file.exists())
        file.remove();

    download->reset();
    Controller::downloadsDao()->update(download);
    foreach(HosterPlugin *hoster, Controller::plugins()->hosterPlugins()) {
        if(hoster->canHandleUrl(download->url())) {
            hoster->getDownloadInformation(download);
            break;
        }
    }

    if(wasRunning
            || isDownloadRunning())
        startDownloads();
}

void DownloadController::resetPackage(DownloadPackage *package)
{
    foreach(Download *dl, package->downloads()) {
        resetDownload(dl);
    }
}

bool DownloadController::isDownloadRunning()
{
    return !m_runningDownloaders.isEmpty();
}

void DownloadController::startDownloads()
{
    while(m_runningDownloaders.size() < Preferences::maxDownloads()) {
        if(!startNextDownload())
            break;
    }
}

void DownloadController::stopDownloads()
{
    foreach(Download *download, m_runningDownloads) {
        stopDownload(download);
    }
}

void DownloadController::stopDownload(Download *download)
{
    if(m_runningDownloaders.contains(download->id())) {
        m_runningDownloaders.value(download->id())->abortDownload();
        m_runningDownloaders.remove(download->id());
        m_runningDownloads.removeAll(download);

        emit statusChanged();
    }
}
