#ifndef DOWNLOADPACKAGE_H
#define DOWNLOADPACKAGE_H

#include <QObject>

#include <QDataSuite/metaobject.h>

#include <QElapsedTimer>
#include <QTime>

class Download;

class DownloadPackage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString message READ message WRITE setMessage)
    Q_PROPERTY(QList<Download*> downloads READ downloads WRITE setDownloads)

    Q_CLASSINFO(QDATASUITE_PRIMARYKEY, "id")
    Q_CLASSINFO("QDATASUITE_PROPERTYMETADATA:id",
                "autoincremented=true;")
    Q_CLASSINFO("QDATASUITE_PROPERTYMETADATA:downloads",
                "reverserelation=package;")

public:
    explicit DownloadPackage(QObject *parent = 0);

    int id() const;

    QString name() const;
    void setName(const QString &name);

    QString message() const;
    void setMessage(const QString &message);

    QList<Download *> downloads() const;
    void addDownload(Download *download);

    int totalFileSize() const;
    int bytesDownloaded() const;

    double progress() const;

    bool isFinished();

    int speed() const;
    int speedWeighted() const;
    QTime eta() const;

private:
    void setId(int id);
    void setDownloads(const QList<Download *> downloads);

    void calculateSpeed() const;

    int m_id;
    QString m_name;
    QString m_message;
    QList<Download *> m_downloads;

    mutable int m_speed;
    mutable int m_weightedSpeed;
    mutable QElapsedTimer m_speedTimer;
    mutable int m_bytesDownloadedAtLastSpeedMeasurement;
    mutable QTime m_eta;

    static float s_speedAlpha;
};

Q_DECLARE_METATYPE(DownloadPackage *)

#endif // DOWNLOADPACKAGE_H
