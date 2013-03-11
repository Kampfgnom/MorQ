#include "series.h"

#include "season.h"

#include <QDebug>

Series::Series(QObject *parent) :
    QObject(parent),
    m_id(-1)
{
}

Series::~Series()
{
    qDebug() << "~Series(" << m_id << ")=" << this;

    foreach(Season *season, m_seasons.values()) {
        season->setSeries(nullptr);
    }
}

int Series::id() const
{
    return m_id;
}

void Series::setId(int id)
{
    m_id = id;
}

QString Series::title() const
{
    return m_title;
}

void Series::setTitle(const QString &title)
{
    m_title = title;
}

QUrl Series::serienJunkiesUrl() const
{
    return m_serienJunkiesUrl;
}

void Series::setSerienJunkiesUrl(const QUrl &url)
{
    m_serienJunkiesUrl = url;
}

QList<Season *> Series::seasons() const
{
    return m_seasons.values();
}

Season *Series::season(int number) const
{
    return m_seasons.value(number);
}

void Series::removeSeason(Season *season)
{
    Q_ASSERT(m_seasons.contains(season->number()));

    season->setSeries(nullptr);
    m_seasons.remove(season->number());
}

void Series::addSeason(Season *season)
{
    Q_ASSERT(!m_seasons.contains(season->number()));

    season->setSeries(this);
    m_seasons.insert(season->number(), season);
}

void Series::setSeasons(const QList<Season *> &seasons)
{
    m_seasons.clear();

    foreach(Season *season, seasons){
        addSeason(season);
    }
}
