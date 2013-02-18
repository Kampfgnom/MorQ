#ifndef DOWNLOADER_H
#define DOWNLOADER_H


#include <QObject>

#include <QSharedDataPointer>

class QUrl;
class QFile;

class DownloaderData;

class Downloader : public QObject
{
    Q_OBJECT
public:
    Downloader(QObject *parent = 0);
    ~Downloader();

    QString destinationFolder() const;
    void setDestinationFolder(const QString destinationFolder);

    QString fileName() const;
    QFile *file() const;

    QUrl url() const;
    void setUrl(const QUrl &url);

    QUrl redirectedUrl() const;
    void setRedirectedUrl(const QUrl &redirectedUrl);

    void getMetaData();
    void startDownload();
    bool isStarted() const;

    int bytesWritten() const;
    int fileSize() const;

    bool isFinished() const;
    bool isDownloading() const;

    QString errorString() const;
    void setErrorString(const QString &errorString);

Q_SIGNALS:
    void started();
    void metaDataChanged();
    void bytesWritten(int bytes);
    void finished();
    void error();


private Q_SLOTS:
    void _metaDataChanged();
    void _readAvailableBytes();
    void _finishedDownload();

private:
    QSharedDataPointer<DownloaderData> data;

    Q_DISABLE_COPY(Downloader)
};

Q_DECLARE_METATYPE(Downloader *)

#endif // DOWNLOADER_H
