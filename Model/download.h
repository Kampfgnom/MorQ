#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QObject>

#include <QDataSuite/metaobject.h>

#include <QElapsedTimer>
#include <QUrl>
#include <QTime>

class Download : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId)
    Q_PROPERTY(QUrl url READ url WRITE setUrl)
    Q_PROPERTY(QString destinationFolder READ destinationFolder WRITE setDestinationFolder)
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName)
    Q_PROPERTY(QString message READ message WRITE setMessage)
    Q_PROPERTY(int fileSize READ fileSize WRITE setFileSize)
    Q_PROPERTY(int bytesDownloaded READ bytesDownloaded WRITE setBytesDownloaded)

    Q_CLASSINFO(QDATASUITE_PRIMARYKEY, "id")
    Q_CLASSINFO("QDATASUITE_PROPERTYMETADATA:id",
                "autoincremented=true;")

public:
    explicit Download(QObject *parent = 0);

    int id() const;
    
    QUrl url() const;
    void setUrl(const QUrl &url);

    QString destinationFolder() const;
    void setDestinationFolder(const QString &destinationFolder);

    QString fileName() const;
    void setFileName(const QString &fileName);

    QString message() const;
    void setMessage(const QString &message);

    int fileSize() const;
    void setFileSize(int bytes);

    int bytesDownloaded() const;
    void setBytesDownloaded(int bytes);

    bool isFinished();

    int speed() const;
    int speedWeighted() const;
    QTime eta() const;


private:
    void setId(int id);

    void calculateSpeed() const;

    int m_id;
    QUrl m_url;
    QString m_destinationFolder;
    QString m_fileName;
    int m_fileSize;
    int m_bytesDownloaded;
    QString m_message;

    mutable int m_speed;
    mutable int m_weightedSpeed;
    mutable QElapsedTimer m_speedTimer;
    mutable int m_bytesDownloadedAtLastSpeedMeasurement;
    mutable QTime m_eta;

    static float s_speedAlpha;
};

Q_DECLARE_METATYPE(Download *)

#endif // DOWNLOAD_H
