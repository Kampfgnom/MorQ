#include "linkscontroller.h"

#include "controller.h"
#include "plugincontroller.h"
#include "downloadcontroller.h"
#include "downloader.h"
#include "plugins/hoster/hosterplugin.h"
#include "plugins/decrypter/decrypterplugin.h"

#include <download.h>
#include <downloadpackage.h>

#include <QApplication>
#include <QClipboard>
#include <QTimer>
#include <QUrl>

LinksController::LinksController(QObject *parent) :
    QObject(parent)
{
    foreach(DecrypterPlugin *decrypter, Controller::plugins()->decrypterPlugins()) {
        connect(decrypter, &DecrypterPlugin::finishedPackage,
                this, &LinksController::packageFinished);
    }

#ifdef Q_OS_MAC
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout,
            this, &LinksController::clipboardChanged);
    timer->setInterval(1000);
    timer->start();
#else
    connect(QApplication::clipboard(), &QClipboard::changed,
            this, &DownloadController::clipboardChanged);
#endif
}

void LinksController::clipboardChanged()
{
    static QString oldClipboard;
    QString clipboard = QApplication::clipboard()->text();
    if(clipboard.isEmpty() || clipboard == oldClipboard)
        return;

    oldClipboard = clipboard;

    QUrl url(clipboard);
    if(!url.isValid())
        return;

    foreach(DecrypterPlugin *decrypter, Controller::plugins()->decrypterPlugins()) {
        if(decrypter->canHandleUrl(url)) {
            DownloadPackage *package = createPackage(url);
            decrypter->handlePackage(package);
            return;
        }
    }

    foreach(HosterPlugin *hoster, Controller::plugins()->hosterPlugins()) {
        if(hoster->canHandleUrl(url)) {
            Download * dl = createDownload(url);
            hoster->getDownloadInformation(dl);
            return;
        }
    }
}

void LinksController::packageFinished(DownloadPackage *package)
{
    foreach(Download *dl, package->downloads()) {
        foreach(HosterPlugin *hoster, Controller::plugins()->hosterPlugins()) {
            if(hoster->canHandleUrl(dl->url())) {
                hoster->getDownloadInformation(dl);
                break;
            }
        }
    }
}

Download *LinksController::createDownload(const QUrl &url)
{
    auto downloadsDao = Controller::downloadsDao();
    Download *download = downloadsDao->create();
    download->setUrl(url);
    downloadsDao->insert(download);

    return download;
}

DownloadPackage *LinksController::createPackage(const QUrl &url)
{
    auto packageDao = Controller::downloadPackagesDao();
    DownloadPackage *package = packageDao->create();
    package->setSourceUrl(url);
    packageDao->insert(package);

    return package;
}
