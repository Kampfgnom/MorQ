#ifndef DOWNLOADCONTROLLER_H
#define DOWNLOADCONTROLLER_H

#include <QObject>

#include <QClipboard>
#include <QTimer>

#include "controller.h"
#include "model/downloadsitemmodel.h"

namespace QPersistence {
template<class T>
class PersistentDataAccessObject;
}

class Download;
class QDownload;
class QShareHoster;

class DownloadController : public QObject
{
    Q_OBJECT
public:
    void addDownload(const QUrl &url);

    DownloadsItemModel *model() const;

private slots:
    void clipboardChanged();
    void metaDataChanged();
    void downloadError();
    void updateDownloads();

private:
    friend class Controller;
    explicit DownloadController(QObject *parent = 0);

    QPersistence::PersistentDataAccessObject<Download> *m_dao;
    DownloadsItemModel *m_model;

    QList<QShareHoster *> m_shareHosters;
    QHash<QDownload *, Download *> m_downloads;

    QTimer m_updateTimer;

    Q_DISABLE_COPY(DownloadController)
};

#endif // DOWNLOADCONTROLLER_H
