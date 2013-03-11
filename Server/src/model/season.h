#ifndef SEASON_H
#define SEASON_H

#include <QObject>

#include <QDataSuite/metaobject.h>

#include <QUrl>

class Series;
class Episode;

class Season : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId)
    Q_PROPERTY(int number READ number WRITE setNumber)
    Q_PROPERTY(QString serienJunkiesTitle READ serienJunkiesTitle WRITE setSerienJunkiesTitle)
    Q_PROPERTY(QUrl serienJunkiesUrl READ serienJunkiesUrl WRITE setSerienJunkiesUrl)
    Q_PROPERTY(Series* series READ series WRITE setSeries)
    Q_PROPERTY(QList<Episode*> episodes READ episodes WRITE setEpisodes)

    Q_CLASSINFO(QDATASUITE_PRIMARYKEY, "id")
    Q_CLASSINFO("QDATASUITE_PROPERTYMETADATA:id",
                "autoincremented=true;")
    Q_CLASSINFO("QDATASUITE_PROPERTYMETADATA:series",
                "reverserelation=seasons;")
    Q_CLASSINFO("QDATASUITE_PROPERTYMETADATA:episodes",
                "reverserelation=season;")

public:
    explicit Season(QObject *parent = 0);
    ~Season();

    int id() const;

    int number() const;
    void setNumber(int number);

    QString serienJunkiesTitle() const;
    void setSerienJunkiesTitle(const QString &title);

    QUrl serienJunkiesUrl() const;
    void setSerienJunkiesUrl(const QUrl &serienJunkiesUrl);

    Series *series() const;

    QList<Episode *> episodes() const;
    Episode *episode(int number) const;
    void addEpisode(Episode *episode);
    void removeEpisode(Episode *episode);

private:
    friend class Series;
    void setId(int id);
    void setSeries(Series *series);
    void setEpisodes(const QList<Episode *> &episodes);

    int m_id;
    int m_number;
    QString m_serienJunkiesTitle;
    QUrl m_serienJunkiesUrl;
    Series *m_series;
    QMap<int, Episode *> m_episodes;
};

#endif // SEASON_H
