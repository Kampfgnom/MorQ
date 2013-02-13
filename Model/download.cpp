#include "download.h"

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
    m_speed(-1),
    m_weightedSpeed(-1),
    m_speedTimer(QElapsedTimer()),
    m_bytesDownloadedAtLastSpeedMeasurement(-1)
{
}

int Download::id() const
{
    return m_id;
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

int Download::fileSize() const
{
    return m_fileSize;
}

void Download::setFileSize(int bytes)
{
    m_fileSize = bytes;
}

int Download::bytesDownloaded() const
{
    return m_bytesDownloaded;
}

void Download::setBytesDownloaded(int bytes)
{
    m_bytesDownloaded = bytes;
}

double Download::progress() const
{
    if(m_bytesDownloaded <= 0
            || m_fileSize <= 0)
        return 0.0;

    return double(m_bytesDownloaded) / double(m_fileSize);
}

bool Download::isFinished()
{
    return fileSize() == bytesDownloaded();
}

void Download::calculateSpeed() const
{
    if(!m_speedTimer.isValid()) {
        m_speedTimer.start();
        return;
    }

    qint64 elapsedTime = m_speedTimer.elapsed();
    if(elapsedTime > 100) {
        int bytesWritten = m_bytesDownloaded - m_bytesDownloadedAtLastSpeedMeasurement;

        if(bytesWritten == 0
                && elapsedTime < 3000)
            return;

        m_bytesDownloadedAtLastSpeedMeasurement = m_bytesDownloaded;
        m_speed = bytesWritten * 1000 / elapsedTime;

        if(m_weightedSpeed == -1)
            m_weightedSpeed = m_speed;
        else
            m_weightedSpeed = m_speed * s_speedAlpha + m_weightedSpeed * (1 - s_speedAlpha);

        m_speedTimer.restart();

        if(m_weightedSpeed <= 0) {
            m_eta = QTime();
            return;
        }

        int bytesLeft = m_fileSize - m_bytesDownloaded;

        if(bytesLeft < 0) {
            m_eta = QTime();
            return;
        }

        if(bytesLeft == 0) {
            m_eta = QTime(0,0,0);
            return;
        }

        m_eta = QTime(0,0,0);
        m_eta = m_eta.addSecs(int(bytesLeft / m_weightedSpeed));
    }
}

int Download::speed() const
{
    calculateSpeed();
    return m_speed;
}

int Download::speedWeighted() const
{
    calculateSpeed();
    return m_weightedSpeed;
}

QTime Download::eta() const
{
    calculateSpeed();
    return m_eta;
}
