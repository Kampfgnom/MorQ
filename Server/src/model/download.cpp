#include "download.h"

#include "downloadpackage.h"

#include <QTime>
#include <QDebug>

float Download::s_speedAlpha(0.1);

Download::Download(QObject *parent) :
    QObject(parent),
    m_id(0),
    m_redirectedUrl(QUrl()),
    m_destinationFolder(QString()),
    m_fileName(QString()),
    m_fileSize(-1),
    m_bytesDownloaded(-1),
    m_package(nullptr),
    m_enabled(true),
    m_extracting(false),
    m_speed(0),
    m_weightedSpeed(0),
    m_speedTimer(QElapsedTimer()),
    m_bytesProcessedAtLastSpeedMeasurement(-1)
{
}

Download::~Download()
{
    qDebug() << "~Download(" << m_id << ")=" << this;
}

int Download::id() const
{
    return m_id;
}

void Download::reset()
{
    m_redirectedUrl = QUrl();
    m_destinationFolder = QString();
    m_fileName = QString();
    m_fileSize = -1;
    m_bytesDownloaded = -1;
    m_enabled = true;
    m_extracting = false;
    m_speed = 0;
    m_weightedSpeed = 0;
    m_bytesProcessedAtLastSpeedMeasurement = -1;
}

QUrl Download::url() const
{
    return m_url;
}

void Download::setUrl(const QUrl &url)
{
    m_url = url;
}

void Download::setId(int id)
{
    m_id = id;
}

QUrl Download::redirectedUrl() const
{
    return m_redirectedUrl;
}

void Download::setRedirectedUrl(const QUrl &url)
{
    m_redirectedUrl = url;
}


QString Download::destinationFolder() const
{
    return m_destinationFolder;
}

void Download::setDestinationFolder(const QString &destinationFolder)
{
    m_destinationFolder = destinationFolder;
}

QString Download::fileName() const
{
    return m_fileName;
}

void Download::setFileName(const QString &fileName)
{
    m_fileName = fileName;
}

QString Download::message() const
{
    return m_message;
}

void Download::setMessage(const QString &message)
{
    m_message = message;
}

DownloadPackage *Download::package() const
{
    return m_package;
}

void Download::setPackage(DownloadPackage *package)
{
    m_package = package;
}

qint64 Download::fileSize() const
{
    return m_fileSize;
}

void Download::setFileSize(qint64 bytes)
{
    m_fileSize = bytes;
}

int Download::order() const
{
    return m_package->downloads().indexOf(const_cast<Download *const>(this));
}

qint64 Download::bytesDownloaded() const
{
    return m_bytesDownloaded;
}

void Download::setBytesDownloaded(qint64 bytes)
{    
    m_bytesDownloaded = bytes;

    if(isDownloadFinished())
        emit downloadFinished();
}

bool Download::isEnabled() const
{
    return m_enabled;
}

void Download::setEnabled(bool e)
{
    m_enabled = e;
    emit enabled(e);
}

double Download::downloadProgress() const
{
    if(m_bytesDownloaded <= 0
            || m_fileSize <= 0)
        return 0.0;

    return double(m_bytesDownloaded) / double(m_fileSize);
}

bool Download::isDownloadFinished() const
{
    return m_fileSize == m_bytesDownloaded;
}

bool Download::isExtracting() const
{
    return m_extracting;
}

void Download::setExtracting(bool extracting)
{
    m_extracting = extracting;
}

void Download::calculateSpeed() const
{
    if(!m_speedTimer.isValid()) {
        m_speedTimer.start();
        return;
    }

    if(isDownloadFinished()) {
        m_speed = 0;
        m_weightedSpeed = 0;
        m_eta = QTime();
        return;
    }

    qint64 elapsedTime = m_speedTimer.elapsed();
    if(elapsedTime > 100) {
        qint64 bytesProcessedSinceLastMeasurement = m_bytesDownloaded - m_bytesProcessedAtLastSpeedMeasurement;

        if(bytesProcessedSinceLastMeasurement == 0
                && elapsedTime < 3000)
            return;

        if(m_bytesProcessedAtLastSpeedMeasurement < 0) {
            m_bytesProcessedAtLastSpeedMeasurement = m_bytesDownloaded;
            return;
        }

        m_bytesProcessedAtLastSpeedMeasurement = m_bytesDownloaded;
        m_speed = bytesProcessedSinceLastMeasurement * 1000 / elapsedTime;
        m_speed = qMax(qint64(0), m_speed);

        if(m_weightedSpeed < 1000) // if weighted is less than 1kb/s just reset it.
            m_weightedSpeed = m_speed;
        else if(m_speed <= 0)
            m_weightedSpeed /= 10;
        else
            m_weightedSpeed = m_speed * s_speedAlpha + m_weightedSpeed * (1 - s_speedAlpha);

        m_weightedSpeed = qMax(qint64(0), m_weightedSpeed);
        m_speedTimer.restart();

        if(m_weightedSpeed <= 0) {
            m_eta = QTime();
            return;
        }

        qint64 bytesLeft = m_fileSize - m_bytesDownloaded;

        if(bytesLeft < 0) {
            m_eta = QTime();
            return;
        }

        if(bytesLeft == 0) {
            m_eta = QTime(0,0,0);
            return;
        }

        m_eta = QTime(0,0,0);
        m_eta = m_eta.addSecs(qint64(bytesLeft / m_weightedSpeed));
    }
}

qint64 Download::speed() const
{
    calculateSpeed();
    return m_speed;
}

qint64 Download::speedWeighted() const
{
    calculateSpeed();
    return m_weightedSpeed;
}

QTime Download::eta() const
{
    calculateSpeed();
    return m_eta;
}
