#include "premiumizemeplugin.h"

#include "controller/controller.h"
#include "controller/downloader.h"

#include <download.h>

#include <QUrlQuery>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QRegularExpression>

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

PremiumizeMeDownloadHandler *PremuimizeMePlugin::handler(Download *download)
{
    PremiumizeMeDownloadHandler *h = m_handlers[download];
    if(!h) {
        h = new PremiumizeMeDownloadHandler(download, this);
        m_handlers.insert(download, h);
    }
    return h;
}

void PremuimizeMePlugin::getDownloadInformation(Download *download)
{
    PremiumizeMeDownloadHandler *h = handler(download);
    h->getDownloadInformation();
}

void PremuimizeMePlugin::handleDownload(Download *download)
{
    PremiumizeMeDownloadHandler *h = handler(download);
    h->download();
}

PremiumizeMeDownloadHandler::PremiumizeMeDownloadHandler(Download *download, PremuimizeMePlugin *parent) :
    QObject(parent),
    m_download(download),
    m_plugin(parent),
    m_downloader(new Downloader(this))
{
}

void PremiumizeMeDownloadHandler::getDownloadInformation()
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
        reply->deleteLater();
    });

    connect(reply, &QNetworkReply::finished,
            this, &PremiumizeMeDownloadHandler::generateLinkReplyFinished);
}

void PremiumizeMeDownloadHandler::download()
{

}

void PremiumizeMeDownloadHandler::generateLinkReplyFinished()
{
    QNetworkReply *reply = static_cast< QNetworkReply *>(sender());

    QString data(QString::fromLatin1(reply->readAll()));
    reply->deleteLater();

    QRegularExpressionMatch match = LOCATION_REGEXP.match(data);
    if(!match.hasMatch()) {
        m_download->setMessage("No download url found: "+data);
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

        m_downloader->deleteLater();
        m_downloader = nullptr;
    });

    QObject::connect(m_downloader, &Downloader::error, [&]() {
        m_download->setFileName(m_downloader->fileName());
        m_download->setMessage(m_downloader->errorString());
        Controller::downloadsDao()->update(m_download);

        m_downloader->deleteLater();
        m_downloader = nullptr;
    });

    // TODO possibly delete this and remove from PremuimizeMePlugin::m_handlers to save memory
}
