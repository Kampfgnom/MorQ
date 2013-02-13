#include "downloader.h"
#include <QSharedData>

#include "controller.h"

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QRegularExpression>
#include <QDir>
#include <QDebug>

class DownloaderData : public QSharedData
{
public:
    DownloaderData() : QSharedData(),
        bytesWritten(-1),
        started(false),
        reply(nullptr),
        file(nullptr),
        fileSize(-1),
        finished(false),
        gettingMetaData(false)
    {}

    QString destinationFolder;
    QString fileName;
    QUrl url;
    QUrl redirectedUrl;
    int bytesWritten;
    bool started;
    QNetworkReply *reply;
    QFile *file;
    int fileSize;
    bool finished;
    Downloader *q;
    QString errorString;
    bool gettingMetaData;

    QString fileNameFromUrl() const;
};

QString DownloaderData::fileNameFromUrl() const
{
    QString path = redirectedUrl.path();
    int index = 0;
    QString result;
    while(result.isEmpty()
          && index >= 0) {
        index = path.lastIndexOf('/', index - 1);
        Q_ASSERT(index >= 0);
        result = path.mid(index);
    }
    return result;
}

Downloader::Downloader(QObject *parent) :
    QObject(parent),
    data(new DownloaderData)
{
    data->q = this;
}

Downloader::~Downloader()
{
    if(data->reply) {
        data->reply->abort();
        data->reply->deleteLater();
    }

    if(data->file) {
        data->file->flush();
        data->file->close();
        data->file->deleteLater();
    }
}

QString Downloader::destinationFolder() const
{
    return data->destinationFolder;
}

void Downloader::setDestinationFolder(const QString destinationFolder)
{
    data->destinationFolder = destinationFolder;
}

QString Downloader::fileName() const
{
    return data->fileName;
}

QFile *Downloader::file() const
{
    return data->file;
}

QUrl Downloader::url() const
{
    return data->url;
}

void Downloader::setUrl(const QUrl &url)
{
    data->url = url;
}

QUrl Downloader::redirectedUrl() const
{
    return data->redirectedUrl;
}

void Downloader::setRedirectedUrl(const QUrl &url)
{
    data->redirectedUrl = url;
}

int Downloader::bytesWritten() const
{
    return data->bytesWritten;
}

int Downloader::fileSize() const
{
    return data->fileSize;
}

bool Downloader::isFinished() const
{
    return data->finished;
}

bool Downloader::isDownloading() const
{
    return data->reply != nullptr;
}

QString Downloader::errorString() const
{
    return data->errorString;
}

void Downloader::setErrorString(const QString &errorString)
{
    data->errorString = errorString;
    if(!errorString.isEmpty())
        emit error();
}

static void (QNetworkReply:: *ERRORSIGNAL)(QNetworkReply::NetworkError) = &QNetworkReply::error;

void Downloader::getMetaData()
{
    if(data->started)
        return;

    if(data->gettingMetaData)
        return;

    data->gettingMetaData = true;
    data->reply = Controller::networkAccessManager()->head(QNetworkRequest(url()));

    QObject::connect(data->reply, &QNetworkReply::metaDataChanged,
                     this, &Downloader::_metaDataChanged);

    QObject::connect(data->reply, ERRORSIGNAL, [=]() {
        setErrorString("Network error: "+data->reply->errorString());
    });
}

void Downloader::startDownload()
{
    if(data->started)
        return;

    data->started = true;

    if(data->reply) { // meta data reply
        data->reply->abort();
        data->reply->disconnect();
        data->reply->deleteLater();
    }

    if(data->redirectedUrl.isEmpty())
        data->redirectedUrl = data->url;

    data->bytesWritten = 0;
    data->reply = Controller::networkAccessManager()->get(QNetworkRequest(redirectedUrl()));

    QObject::connect(data->reply, &QNetworkReply::readyRead,
                     this, &Downloader::_readAvailableBytes);
    QObject::connect(data->reply, &QNetworkReply::finished,
                     this, &Downloader::_finishedDownload);
    QObject::connect(data->reply, &QNetworkReply::metaDataChanged,
                     this, &Downloader::_metaDataChanged);

    QObject::connect(data->reply, ERRORSIGNAL, [=]() {
        setErrorString("Network error: "+data->reply->errorString());
    });
    emit started();
}

bool Downloader::isStarted() const
{
    return data->started;
}

void Downloader::_metaDataChanged()
{
    data->fileSize = data->reply->header(QNetworkRequest::ContentLengthHeader).toInt();

    // FileName
    QString contentDisposition = QString::fromLatin1(data->reply->rawHeader("Content-Disposition"));
    if(!contentDisposition.isEmpty()) {
        QRegularExpression reg(QLatin1String("filename=\\\"(.*)\\\""));
        QRegularExpressionMatch match = reg.match(contentDisposition);
        if(match.hasMatch()) {
            QString name = match.captured(1);
            if(!name.isEmpty())
                data->fileName = name;
        }
    }
    if(data->fileName.isEmpty())
        data->fileName = data->fileNameFromUrl();
    Q_ASSERT(!data->fileName.isEmpty());


    // Redirection
    data->redirectedUrl = data->reply->header(QNetworkRequest::LocationHeader).toUrl();

    if(data->redirectedUrl.isEmpty())
        data->redirectedUrl = data->url;

    if(data->reply->operation() == QNetworkAccessManager::HeadOperation) {
        data->reply->deleteLater();
        data->reply = nullptr;
    }

    emit metaDataChanged();
}

void Downloader::_readAvailableBytes()
{
    if(!data->file) {
        data->file = new QFile(data->destinationFolder + QDir::separator() + data->fileName);
        if(!data->file->open(QIODevice::WriteOnly)) {
            setErrorString(QString("Could not write to file '%1': %2")
                           .arg(data->file->fileName())
                           .arg(data->file->errorString()));
            return;
        }
    }

    int bytesAvailable = data->reply->bytesAvailable();
    if(bytesAvailable < (2 << 14))
        return;

    data->file->write(data->reply->read(bytesAvailable));

    data->bytesWritten += bytesAvailable;
    emit bytesWritten(bytesAvailable);
}

void Downloader::_finishedDownload()
{
    int bytesAvailable = data->reply->bytesAvailable();

    data->bytesWritten += bytesAvailable;
    data->file->write(data->reply->readAll());

    emit bytesWritten(bytesAvailable);

    data->reply->deleteLater();
    data->file->flush();
    data->file->deleteLater();

    data->reply = nullptr;
    data->file = nullptr;
    data->finished = true;

    emit finished();
}
