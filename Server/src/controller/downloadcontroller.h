#ifndef DOWNLOADCONTROLLER_H
#define DOWNLOADCONTROLLER_H

#include <QObject>
#include <QHash>

class Downloader;
class Download;
class DownloadPackage;

class DownloadController : public QObject
{
    Q_OBJECT
public:
    void startDownloads();
    void stopDownloads();
    void stopDownload(Download *download);
    void removeDownload(Download *download);
    void removePackage(DownloadPackage *package);
    void resetDownload(Download *download);
    void resetPackage(DownloadPackage *package);

    bool isDownloadRunning();

signals:
    void statusChanged();

private:
    friend class Controller;
    explicit DownloadController(QObject *parent = 0);

    bool startNextDownload();

    Download *findNextUnfinishedDownload();

    QList<Download *> m_runningDownloads;
    QHash<int, Downloader *> m_runningDownloaders;

    Q_DISABLE_COPY(DownloadController)
};

#endif // DOWNLOADCONTROLLER_H
