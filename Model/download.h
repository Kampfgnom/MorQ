#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QObject>

#include <QDataSuite/metaobject.h>

#include <QElapsedTimer>
#include <QUrl>
#include <QTime>

class DownloadPackage;

class Download : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId)
    Q_PROPERTY(QUrl url READ url WRITE setUrl)
    Q_PROPERTY(QUrl redirectedUrl READ redirectedUrl WRITE setRedirectedUrl)
    Q_PROPERTY(QString destinationFolder READ destinationFolder WRITE setDestinationFolder)
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName)
    Q_PROPERTY(QString message READ message WRITE setMessage)
    Q_PROPERTY(DownloadPackage* package READ package WRITE setPackage)
    Q_PROPERTY(int fileSize READ fileSize WRITE setFileSize)
    Q_PROPERTY(int bytesDownloaded READ bytesDownloaded WRITE setBytesDownloaded)

    Q_CLASSINFO(QDATASUITE_PRIMARYKEY, "id")
    Q_CLASSINFO("QDATASUITE_PROPERTYMETADATA:id",
                "autoincremented=true;")
    Q_CLASSINFO("QDATASUITE_PROPERTYMETADATA:package",
                "reverserelation=downloads;")

public:
    explicit Download(QObject *parent = 0);

    int id() const;
    
    QUrl url() const;
    void setUrl(const QUrl &redirectedUrl);

    QUrl redirectedUrl() const;
    void setRedirectedUrl(const QUrl &redirectedUrl);

    QString destinationFolder() const;
    void setDestinationFolder(const QString &destinationFolder);

    QString fileName() const;
    void setFileName(const QString &fileName);

    QString message() const;
    void setMessage(const QString &message);

    DownloadPackage *package() const;

    int fileSize() const;
    void setFileSize(int bytes);

    int bytesDownloaded() const;
    void setBytesDownloaded(int bytes);

    double progress() const;

    bool isFinished();

    int speed() const;
    int speedWeighted() const;
    QTime eta() const;


private:
    void setId(int id);
    void setPackage(DownloadPackage *package);

    void calculateSpeed() const;

    int m_id;
    QUrl m_url;
    QUrl m_redirectedUrl;
    QString m_destinationFolder;
    QString m_fileName;
    int m_fileSize;
    int m_bytesDownloaded;
    QString m_message;
    DownloadPackage *m_package;

    mutable int m_speed;
    mutable int m_weightedSpeed;
    mutable QElapsedTimer m_speedTimer;
    mutable int m_bytesDownloadedAtLastSpeedMeasurement;
    mutable QTime m_eta;

    static float s_speedAlpha;
};

Q_DECLARE_METATYPE(Download *)

#endif // DOWNLOAD_H
