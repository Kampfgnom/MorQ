#include "premiumizemeplugin.h"

#include "controller/controller.h"
#include "controller/downloader.h"
#include "preferences.h"

#include <download.h>

#include <QUrlQuery>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QRegularExpression>
#include <QTimer>

static const QRegularExpression LOCATION_REGEXP(QLatin1String("location\":\"(http[^\"]+)"));
static void (QNetworkReply:: *ERRORSIGNAL)(QNetworkReply::NetworkError) = &QNetworkReply::error;

PremuimizeMePlugin::PremuimizeMePlugin(QObject *parent) :
    HosterPlugin(parent)
{
}

bool PremuimizeMePlugin::canHandleUrl(const QUrl &url) const
{
    if(!url.isValid())
        return false;

    if(url.host() == QLatin1String("uploaded.to")
            || url.host() == QLatin1String("uploaded.net"))
        return true;

    return false;
}

void PremuimizeMePlugin::getDownloadInformation(Download *download)
{
    PremiumizeMeDownloadHandler *h = new PremiumizeMeDownloadHandler(download, this);
    h->getDownloadInformation();
}

Downloader *PremuimizeMePlugin::handleDownload(Download *download)
{
    PremiumizeMeDownloadHandler *h = new PremiumizeMeDownloadHandler(download, this);
    h->download();
    return h->downloader();
}

PremiumizeMeDownloadHandler::PremiumizeMeDownloadHandler(Download *download, PremuimizeMePlugin *parent) :
    QObject(parent),
    m_download(download),
    m_plugin(parent),
    m_downloader(new Downloader(this))
{
    QObject::connect(m_downloader, &Downloader::error, [&]() {
        m_download->setFileName(m_downloader->fileName());
        m_download->setMessage(m_downloader->errorString());
        m_download->setEnabled(false);
        Controller::downloadsDao()->update(m_download);

        this->deleteLater();
    });

    if(!s_timer.isActive()) {
        s_timer.setInterval(1000);
        s_timer.start();
    }
}

PremiumizeMeDownloadHandler::~PremiumizeMeDownloadHandler()
{
}

QNetworkReply *PremiumizeMeDownloadHandler::getDownloadInformationReply()
{
    QUrlQuery query;
    query.addQueryItem("method", "directdownloadlink");
    query.addQueryItem("params[login]", m_plugin->account().userName);
    query.addQueryItem("params[pass]", m_plugin->account().password);
    query.addQueryItem("params[link]", m_download->url().toEncoded());

    QUrl getLinkUrl(QString::fromLatin1("https://api.premiumize.me/pm-api/v1.php"));
    getLinkUrl.setQuery(query);

    m_download->setMessage("Generating url...");
    Controller::downloadsDao()->update(m_download);

    QNetworkReply *reply = Controller::networkAccessManager()->get(QNetworkRequest(getLinkUrl));

    QObject::connect(reply, ERRORSIGNAL, [=]() {
        m_download->setMessage("Network Error: "+reply->errorString());
        m_download->setEnabled(false);
        reply->deleteLater();
    });

    return reply;
}

void PremiumizeMeDownloadHandler::getDownloadInformation()
{
    QNetworkReply *reply = getDownloadInformationReply();

    connect(reply, &QNetworkReply::finished,
            this, &PremiumizeMeDownloadHandler::generateLinkReplyFinished);

    connect(this, &PremiumizeMeDownloadHandler::downloadInformationReady, [=]() {
        this->deleteLater();
    });
}

void PremiumizeMeDownloadHandler::download()
{
    QNetworkReply *reply = getDownloadInformationReply();

    connect(reply, &QNetworkReply::finished,
            this, &PremiumizeMeDownloadHandler::generateLinkReplyFinished);

    connect(this, &PremiumizeMeDownloadHandler::downloadInformationReady, [=]() {
        m_downloader->setDestinationFolder(Preferences::downloadFolder());
        m_downloader->startDownload();
    });

    auto connection = QObject::connect(&s_timer, &QTimer::timeout, [=]() {
        m_download->setBytesDownloaded(m_downloader->bytesWritten());
        Controller::downloadsDao()->update(m_download);
    });

    QObject::connect(m_downloader, &Downloader::error, [=]() {
        disconnect(connection);
    });

    QObject::connect(this, &QObject::destroyed, [=]() {
        disconnect(connection);
    });

    QObject::connect(m_downloader, &Downloader::finished, [=]() {
        m_download->setBytesDownloaded(m_downloader->bytesWritten());
        Controller::downloadsDao()->update(m_download);
        this->deleteLater();
    });
}

Downloader *PremiumizeMeDownloadHandler::downloader() const
{
    return m_downloader;
}

void PremiumizeMeDownloadHandler::generateLinkReplyFinished()
{
    QNetworkReply *reply = static_cast< QNetworkReply *>(sender());

    QString data(QString::fromLatin1(reply->readAll()));
    reply->deleteLater();

    QRegularExpressionMatch match = LOCATION_REGEXP.match(data);
    if(!match.hasMatch()) {
        m_download->setMessage("No download url found: "+data);
        m_download->setEnabled(false);
        return;
    }

    QString downloadUrl = match.captured(1);
    downloadUrl.replace(QLatin1String("\\/"), QLatin1String("/"));

    m_download->setRedirectedUrl(QUrl(downloadUrl));
    m_download->setMessage("Getting file information...");
    Controller::downloadsDao()->update(m_download);

    m_downloader->setUrl(m_download->redirectedUrl());
    m_downloader->getMetaData();

    QObject::connect(m_downloader, &Downloader::metaDataChanged, [&]() {
        m_download->setRedirectedUrl(m_downloader->redirectedUrl());
        m_download->setFileName(m_downloader->fileName());
        m_download->setFileSize(m_downloader->fileSize());
        m_download->setMessage("");
        Controller::downloadsDao()->update(m_download);

        emit downloadInformationReady();
    });
}

QTimer PremiumizeMeDownloadHandler::s_timer;
