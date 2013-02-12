#include "downloadcontroller.h"

#include "preferences.h"

#include <download.h>

#include <qdownload.h>
#include <qsharehoster.h>
#include <premuimizemehoster.h>

#include <QPersistence/persistentdataaccessobject.h>
#include <QPersistence/databaseschema.h>

#include <QApplication>
#include <QDebug>
#include <QTimer>

DownloadController::DownloadController(QObject *parent) :
    QObject(parent)
{
    QDataSuite::registerMetaObject<Download>();
    m_dao = new QPersistence::PersistentDataAccessObject<Download>;
    QPersistence::registerPersistentDataAccessObject<Download>(m_dao);


    QPersistence::DatabaseSchema databaseSchema;
    databaseSchema.dropTable(&Download::staticMetaObject);
    databaseSchema.createTable(&Download::staticMetaObject);

    m_model = new DownloadsItemModel(m_dao, this);

    PremuimizeMeHoster *premiumizeMe = new PremuimizeMeHoster(this);
    premiumizeMe->setUserName(Preferences::premiumizeMeUserName());
    premiumizeMe->setPassword(Preferences::premiumizeMeUserPassword());
    m_shareHosters.append(premiumizeMe);

    m_updateTimer.setInterval(1000);
    connect(&m_updateTimer, &QTimer::timeout,
            this, &DownloadController::updateDownloads);
    m_updateTimer.start();

#ifdef Q_OS_MAC
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout,
            this, &DownloadController::clipboardChanged);
    timer->setInterval(1000);
    timer->start();
#else
    connect(QApplication::clipboard(), &QClipboard::changed,
            this, &DownloadController::clipboardChanged);
#endif
}

void DownloadController::addDownload(const QUrl &url)
{
    foreach(QShareHoster *hoster, m_shareHosters) {
        if(hoster->canHandleUrl(url)) {
            QDownload *download = hoster->createDownload(url);
            download->setParent(this);
            connect(download, &QDownload::metaDataChanged,
                    this, &DownloadController::metaDataChanged);
            connect(download, &QDownload::error,
                    this, &DownloadController::downloadError);
            break;
        }
    }

}

DownloadsItemModel *DownloadController::model() const
{
    return m_model;
}

void DownloadController::clipboardChanged()
{
    static QString oldClipboard;
    QString clipboard = QApplication::clipboard()->text();
    if(!clipboard.isEmpty()
            && clipboard != oldClipboard) {
        oldClipboard = clipboard;

        QUrl url(clipboard);

        foreach(QShareHoster *hoster, m_shareHosters) {
            if(hoster->canHandleUrl(url)) {
                addDownload(url);
                break;
            }
        }
    }
}

void DownloadController::metaDataChanged()
{
    QDownload *qDownload = static_cast<QDownload *>(sender());

    Download *download = nullptr;
    if(m_downloads.contains(qDownload)) {
        download = m_downloads.value(qDownload);
    }
    else {
        download = m_dao->create();
    }

    download->setParent(this);
    download->setRedirectedUrl(qDownload->redirectedUrl());
    download->setUrl(qDownload->url());
    download->setFileName(qDownload->fileName());
    download->setFileSize(qDownload->fileSize());

    if(m_downloads.contains(qDownload)) {
        m_dao->update(download);
    }
    else {
        m_dao->insert(download);
        m_downloads.insert(qDownload, download);
    }
}

void DownloadController::downloadError()
{
    QDownload *qDownload = static_cast<QDownload *>(sender());

    if(m_downloads.contains(qDownload)) {
        Download *download = m_downloads.value(qDownload);
        download->setMessage(qDownload->errorString());
        m_dao->update(download);
    }

    if(qDownload) {
        qDownload->deleteLater();
    }
}

void DownloadController::updateDownloads()
{
    QHashIterator<QDownload *, Download *> it(m_downloads);
    while(it.hasNext()) {
        it.next();
        QDownload *qdl = it.key();
        Download *dl = it.value();
        int oldBytes = dl->bytesDownloaded();
        int newBytes = qdl->bytesWritten();
        if(oldBytes != newBytes) {
            dl->setBytesDownloaded(newBytes);
            m_dao->update(dl);
        }
    }
}
