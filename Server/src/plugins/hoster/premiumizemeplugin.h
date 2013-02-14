#ifndef PREMIUMIZEMEPLUGIN_H
#define QSHAREHOSTERPLUGIN_H

#include "hosterplugin.h"

#include <QHash>

class QNetworkReply;
class Download;
class PremiumizeMeDownloadHandler;
class Downloader;

class PremuimizeMePlugin : public HosterPlugin
{
    Q_OBJECT
public:
    explicit PremuimizeMePlugin(QObject *parent = 0);

    void getDownloadInformation(Download *download);
    void handleDownload(Download *download);
    bool canHandleUrl(const QUrl &url) const;

private:
    QHash<Download *, PremiumizeMeDownloadHandler *> m_handlers;

    PremiumizeMeDownloadHandler *handler(Download *download);
};

class PremiumizeMeDownloadHandler : public QObject
{
    Q_OBJECT
public:
    PremiumizeMeDownloadHandler(Download *download, PremuimizeMePlugin *parent);
    void getDownloadInformation();
    void download();

private slots:
    void generateLinkReplyFinished();

private:
    Download *m_download;
    PremuimizeMePlugin *m_plugin;
    Downloader *m_downloader;
};

#endif // PREMIUMIZEMEPLUGIN_H
