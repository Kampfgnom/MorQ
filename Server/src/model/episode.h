#ifndef EPISODE_H
#define EPISODE_H

#include <QObject>

#include <QDataSuite/metaobject.h>

class VideoDownloadLink;
class Season;

class Episode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId)
    Q_PROPERTY(int number READ number WRITE setNumber)
    Q_PROPERTY(QString serienJunkiesTitle READ serienJunkiesTitle WRITE setSerienJunkiesTitle)
    Q_PROPERTY(Season* season READ season WRITE setSeason)
    Q_PROPERTY(QList<VideoDownloadLink*> downloadLinks READ downloadLinks WRITE setDownloadLinks)

    Q_CLASSINFO(QDATASUITE_PRIMARYKEY, "id")
    Q_CLASSINFO("QDATASUITE_PROPERTYMETADATA:id",
                "autoincremented=true;")

    Q_CLASSINFO("QDATASUITE_PROPERTYMETADATA:season",
                "reverserelation=episodes;")

    Q_CLASSINFO("QDATASUITE_PROPERTYMETADATA:downloadLinks",
                "reverserelation=episode;")


public:
    explicit Episode(QObject *parent = 0);
    ~Episode();

    int id() const;

    int number() const;
    void setNumber(int number);

    QString serienJunkiesTitle() const;
    void setSerienJunkiesTitle(const QString &title);

    Season *season() const;

    QList<VideoDownloadLink *> downloadLinks() const;
    void addDownloadLink(VideoDownloadLink *link);
    void removeDownloadLink(VideoDownloadLink *link);

private:
    friend class Season;
    void setId(int id);
    void setSeason(Season *season);
    void setDownloadLinks(const QList<VideoDownloadLink *> &links);

    int m_id;
    int m_number;
    QString m_serienJunkiesTitle;
    Season *m_season;
    QList<VideoDownloadLink *> m_downloadLinks;
};

#endif // EPISODE_H
