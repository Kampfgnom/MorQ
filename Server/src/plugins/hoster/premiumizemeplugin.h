#ifndef PREMIUMIZEMEPLUGIN_H
#define QSHAREHOSTERPLUGIN_H

#include "hosterplugin.h"

#include <QHash>

class QNetworkReply;
class Download;
class PremiumizeMeDownloadHandler;
class Downloader;
class QTimer;

class PremuimizeMePlugin : public HosterPlugin
{
    Q_OBJECT
public:
    explicit PremuimizeMePlugin(QObject *parent = 0);

    void getDownloadInformation(Download *download);
    Downloader *handleDownload(Download *download);
    bool canHandleUrl(const QUrl &url) const;
};

class PremiumizeMeDownloadHandler : public QObject
{
    Q_OBJECT
public:
    PremiumizeMeDownloadHandler(Download *download, PremuimizeMePlugin *parent);
    ~PremiumizeMeDownloadHandler();
    void getDownloadInformation();
    void download();

    Downloader *downloader() const;

signals:
    void downloadInformationReady();

private slots:
    void generateLinkReplyFinished();

private:
    Download *m_download;
    PremuimizeMePlugin *m_plugin;
    Downloader *m_downloader;
    static QTimer s_timer;

    QNetworkReply *getDownloadInformationReply();
};

#endif // PREMIUMIZEMEPLUGIN_H
