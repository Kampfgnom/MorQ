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
    void stopDownload(Download *download);
    void removeDownload(Download *download);
    void removePackage(DownloadPackage *package);

private:
    friend class Controller;
    explicit DownloadController(QObject *parent = 0);

    bool startNextDownload();

    Download *findNextUnfinishedDownload();

    QHash<int, Downloader *> m_runningDownloads;

    Q_DISABLE_COPY(DownloadController)
};

#endif // DOWNLOADCONTROLLER_H
