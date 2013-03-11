#ifndef SERIES_H
#define SERIES_H

#include <QObject>

#include <QDataSuite/metaobject.h>

#include <QUrl>

class Season;

class Series : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QUrl serienJunkiesUrl READ serienJunkiesUrl WRITE setSerienJunkiesUrl)
    Q_PROPERTY(QList<Season*> seasons READ seasons WRITE setSeasons)

    Q_CLASSINFO(QDATASUITE_PRIMARYKEY, "id")
    Q_CLASSINFO("QDATASUITE_PROPERTYMETADATA:id",
                "autoincremented=true;")
    Q_CLASSINFO("QDATASUITE_PROPERTYMETADATA:seasons",
                "reverserelation=series;")

public:
    explicit Series(QObject *parent = 0);
    ~Series();
    
    int id() const;

    QString title() const;
    void setTitle(const QString &title);

    QUrl serienJunkiesUrl() const;
    void setSerienJunkiesUrl(const QUrl &serienJunkiesUrl);

    QList<Season *> seasons() const;
    Season *season(int number) const;
    void removeSeason(Season *season);
    void addSeason(Season *season);

private:
    void setId(int id);
    void setSeasons(const QList<Season *> &seasons);

    int m_id;
    QString m_title;
    QUrl m_serienJunkiesUrl;
    QMap<int, Season *> m_seasons;
};

#endif // SERIES_H
