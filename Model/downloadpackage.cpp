#include "downloadpackage.h"

#include <QTime>
#include <QDebug>

float DownloadPackage::s_speedAlpha(0.1);

DownloadPackage::DownloadPackage(QObject *parent) :
    QObject(parent),
    m_id(0),
    m_speed(-1),
    m_weightedSpeed(-1),
    m_speedTimer(QElapsedTimer()),
    m_bytesDownloadedAtLastSpeedMeasurement(-1)
{
}

int DownloadPackage::id() const
{
    return m_id;
}

void DownloadPackage::setId(int id)
{
    m_id = id;
}

QString DownloadPackage::name() const
{
    return m_name;
}

void DownloadPackage::setName(const QString &name)
{
    m_name = name;
}

QString DownloadPackage::message() const
{
    return m_message;
}

void DownloadPackage::setMessage(const QString &message)
{
    m_message = message;
}

QList<Download *> DownloadPackage::downloads() const
{
    return m_downloads;
}

void DownloadPackage::addDownload(Download *download)
{
    m_downloads.append(download);
}

void DownloadPackage::setDownloads(const QList<Download *> downloads)
{
    m_downloads = downloads;
}

int DownloadPackage::totalFileSize() const
{
    // TODO aggregate download bytes totalFileSize
    return 0;
}

int DownloadPackage::bytesDownloaded() const
{
    // TODO aggregate download bytesDownloaded
    return 0;
}

double DownloadPackage::progress() const
{
    return double(bytesDownloaded()) / double(totalFileSize());
}

bool DownloadPackage::isFinished()
{
    return totalFileSize() == bytesDownloaded();
}

void DownloadPackage::calculateSpeed() const
{
    if(!m_speedTimer.isValid()) {
        m_speedTimer.start();
        return;
    }

    qint64 elapsedTime = m_speedTimer.elapsed();
    if(elapsedTime > 100) {
        int bytes = bytesDownloaded();
        int bytesWritten = bytes - m_bytesDownloadedAtLastSpeedMeasurement;

        if(bytesWritten == 0
                && elapsedTime < 3000)
            return;

        m_bytesDownloadedAtLastSpeedMeasurement = bytes;
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

        int bytesLeft = totalFileSize() - bytes;

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

int DownloadPackage::speed() const
{
    calculateSpeed();
    return m_speed;
}

int DownloadPackage::speedWeighted() const
{
    calculateSpeed();
    return m_weightedSpeed;
}

QTime DownloadPackage::eta() const
{
    calculateSpeed();
    return m_eta;
}
