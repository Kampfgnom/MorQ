#ifndef DOWNLOADCONTROLLER_H
#define DOWNLOADCONTROLLER_H

#include <QObject>
#include <QHash>

class Downloader;
class Download;

class DownloadController : public QObject
{
    Q_OBJECT
public:
    void startDownloads();

private:
    friend class Controller;
    explicit DownloadController(QObject *parent = 0);

    void startNextDownload();

    Download *findNextUnfinishedDownload();

    QHash<int, bool> m_runningDownloads;

    Q_DISABLE_COPY(DownloadController)
};

#endif // DOWNLOADCONTROLLER_H
