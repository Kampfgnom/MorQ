#include "downloadpackage.h"

#include "download.h"

#include <QTime>
#include <QDebug>

float DownloadPackage::s_speedAlpha(0.1);

DownloadPackage::DownloadPackage(QObject *parent) :
    QObject(parent),
    m_id(0),
    m_speed(0),
    m_weightedSpeed(0),
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

QUrl DownloadPackage::sourceUrl() const
{
    return m_sourceUrl;
}

void DownloadPackage::setSourceUrl(const QUrl &url)
{
    m_sourceUrl = url;
}

QList<Download *> DownloadPackage::downloads() const
{
    return m_downloads;
}

void DownloadPackage::addDownload(Download *download)
{
    download->setPackage(this);
    m_downloads.append(download);
}

QByteArray DownloadPackage::captcha() const
{
    return m_captcha;
}

void DownloadPackage::setCaptcha(const QByteArray &captcha)
{
    m_captcha = captcha;
}

QString DownloadPackage::captchaString() const
{
    return m_captchaString;
}

void DownloadPackage::setCaptchaString(const QString &string)
{
    m_captchaString = string;
    emit captchaStringChanged();
}

void DownloadPackage::setDownloads(const QList<Download *> downloads)
{
    m_downloads = downloads;
}

qint64 DownloadPackage::totalFileSize() const
{
    qint64 total = 0;
    foreach(Download *dl, downloads()) {
        total += dl->fileSize();
    }

    return total;
}

qint64 DownloadPackage::bytesDownloaded() const
{
    qint64 total = 0;
    foreach(Download *dl, downloads()) {
        total += dl->bytesDownloaded();
    }

    return total;
}

double DownloadPackage::progress() const
{
    qint64 byteDown = bytesDownloaded();
    qint64 total = totalFileSize();
    if(byteDown <= 0
            || total <= 0)
        return 0.0;

    return double(byteDown) / double(total);
}

bool DownloadPackage::isFinished() const
{
    return totalFileSize() == bytesDownloaded();
}

void DownloadPackage::calculateSpeed() const
{
    if(!m_speedTimer.isValid()) {
        m_speedTimer.start();
        return;
    }

    if(isFinished()) {
        m_speed = 0;
        m_weightedSpeed = 0;
        m_eta = QTime();
        return;
    }


    qint64 elapsedTime = m_speedTimer.elapsed();
    if(elapsedTime > 100) {
        qint64 bytes = bytesDownloaded();
        qint64 bytesWritten = bytes - m_bytesDownloadedAtLastSpeedMeasurement;

        if(bytesWritten == 0
                && elapsedTime < 3000)
            return;

        if(m_bytesDownloadedAtLastSpeedMeasurement < 0) {
            m_bytesDownloadedAtLastSpeedMeasurement = bytes;
            return;
        }

        m_bytesDownloadedAtLastSpeedMeasurement = bytes;

        m_speed = bytesWritten * 1000 / elapsedTime;
        m_speed = qMax(qint64(0), m_speed);

        if(m_weightedSpeed < 1000) // if weighted is less than 1kb/s just reset it.
            m_weightedSpeed = m_speed;
        else if(m_speed <= 0)
            m_weightedSpeed /= 10;
        else
            m_weightedSpeed = m_speed * s_speedAlpha + m_weightedSpeed * (1 - s_speedAlpha);

        m_speed = qMax(qint64(0), m_speed);
        m_speedTimer.restart();

        if(m_weightedSpeed <= 0) {
            m_eta = QTime();
            return;
        }

        qint64 bytesLeft = totalFileSize() - bytes;

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

qint64 DownloadPackage::speed() const
{
    calculateSpeed();
    return m_speed;
}

qint64 DownloadPackage::speedWeighted() const
{
    calculateSpeed();
    return m_weightedSpeed;
}

QTime DownloadPackage::eta() const
{
    calculateSpeed();
    return m_eta;
}
