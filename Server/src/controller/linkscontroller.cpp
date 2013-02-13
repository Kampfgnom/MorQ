#include "linkscontroller.h"

#include "controller.h"
#include "plugincontroller.h"
#include "downloadcontroller.h"
#include "downloader.h"
#include "plugins/hoster/hosterplugin.h"

#include <QApplication>
#include <QClipboard>
#include <QTimer>

LinksController::LinksController(QObject *parent) :
    QObject(parent)
{
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

    foreach(HosterPlugin *hoster, Controller::plugins()->hosterPlugins()) {
        if(hoster->canHandleUrl(url)) {
            Download * dl = createDownload(url);
            hoster->getDownloadInformation(dl);
        }
    }
}

Download *LinksController::createDownload(const QUrl &url)
{
    auto downloadsDao = Controller::downloadsDao();
    Download *download = downloadsDao->create();
    download->setUrl(url);
    download->setParent(this);
    downloadsDao->insert(download);

    return download;
}
