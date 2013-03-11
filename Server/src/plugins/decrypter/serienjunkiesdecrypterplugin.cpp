#include "serienjunkiesdecrypterplugin.h"

#include "controller/controller.h"
#include "controller/linkscontroller.h"

#include "model/downloadpackage.h"
#include "model/download.h"

#include <qserienjunkies.h>

#include <QUrl>
#include <QRegularExpression>
#include <QDebug>

static QRegularExpression LINK_REGEXP("http://[\\w\\.]*?serienjunkies\\.org/.*?/[a-z]{2}[_-].*");

SerienJunkiesDecrypterPlugin::SerienJunkiesDecrypterPlugin(QObject *parent) :
    DecrypterPlugin(parent)
{
    QSerienJunkies::setNetworkAccessManager(Controller::networkAccessManager());
}

bool SerienJunkiesDecrypterPlugin::canHandleUrl(const QUrl &url) const
{
    if(!url.isValid())
        return false;

    if(LINK_REGEXP.match(url.toString()).hasMatch())
        return true;

    return false;
}

void SerienJunkiesDecrypterPlugin::handlePackage(DownloadPackage *package)
{
    SerienJunkiesDecryptHandler *handler = new SerienJunkiesDecryptHandler(package, this);
    package->setName(package->sourceUrl().path().split('/').last());
    handler->getInformation();
    connect(handler, &SerienJunkiesDecryptHandler::finished,
            this, &SerienJunkiesDecrypterPlugin::packageFinished);
}


SerienJunkiesDecryptHandler::SerienJunkiesDecryptHandler(DownloadPackage *package,
                                                         SerienJunkiesDecrypterPlugin *parent) :
    QObject(parent),
    m_package(package),
    m_plugin(parent)
{
}

void SerienJunkiesDecryptHandler::getInformation()
{
    QSerienJunkiesReply *reply = QSerienJunkies::decrypt(m_package->sourceUrl());

    connect(reply, &QSerienJunkiesReply::error, [=]() {
        m_package->setMessage(reply->errorString());
        Controller::downloadPackagesDao()->update(m_package);
    });

    connect(reply, &QSerienJunkiesReply::requiresCaptcha,
            this, &SerienJunkiesDecryptHandler::requestCaptchaSolution);

    connect(reply, &QSerienJunkiesReply::finished,
            this, &SerienJunkiesDecryptHandler::replyFinished);
}

DownloadPackage *SerienJunkiesDecryptHandler::package() const
{
    return m_package;
}

void SerienJunkiesDecryptHandler::requestCaptchaSolution()
{
    QSerienJunkiesReply *reply = static_cast<QSerienJunkiesReply *>(sender());

    m_package->setName(reply->packageName());
    m_package->setCaptcha(reply->captcha());

    connect(m_package, &DownloadPackage::captchaStringChanged, [=]() {
        QString solution = m_package->captchaString();
        if(!solution.isEmpty())
            reply->solveCaptcha(solution);
    });

    Controller::downloadPackagesDao()->update(m_package);
}

void SerienJunkiesDecryptHandler::replyFinished()
{
    QSerienJunkiesReply *reply = static_cast<QSerienJunkiesReply *>(sender());
    m_package->setCaptcha(QByteArray());

    foreach(const QUrl &url, reply->urls()) {
        Download *download = Controller::links()->createDownload(url);
        m_package->addDownload(download);
        m_package->setMessage(QString());
        Controller::downloadsDao()->update(download);
        Controller::downloadPackagesDao()->update(m_package);
    }

    emit finished();
}


void SerienJunkiesDecrypterPlugin::packageFinished()
{
    SerienJunkiesDecryptHandler *handler = static_cast<SerienJunkiesDecryptHandler *>(sender());
    emit finishedPackage(handler->package());
    handler->deleteLater();
}
