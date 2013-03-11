#include "season.h"

#include "episode.h"
#include "series.h"

#include <QDebug>

Season::Season(QObject *parent) :
    QObject(parent),
    m_id(-1),
    m_number(0),
    m_series(nullptr)
{
}

Season::~Season()
{
    qDebug() << "~Season(" << m_id << ")=" << this;

    if(m_series) {
        m_series->removeSeason(this);
    }

    foreach(Episode *episode, m_episodes.values()) {
        episode->setSeason(nullptr);
    }
}

int Season::id() const
{
    return m_id;
}

void Season::setId(int id)
{
    m_id = id;
}

int Season::number() const
{
    return m_number;
}

void Season::setNumber(int number)
{
    m_number = number;
}

QString Season::serienJunkiesTitle() const
{
    return m_serienJunkiesTitle;
}

void Season::setSerienJunkiesTitle(const QString &title)
{
    m_serienJunkiesTitle = title;
}

QUrl Season::serienJunkiesUrl() const
{
    return m_serienJunkiesUrl;
}

void Season::setSerienJunkiesUrl(const QUrl &serienJunkiesUrl)
{
    m_serienJunkiesUrl = serienJunkiesUrl;
}

Series *Season::series() const
{
    return m_series;
}

void Season::setSeries(Series *series)
{
    m_series = series;
}


QList<Episode *> Season::episodes() const
{
    return m_episodes.values();
}

Episode *Season::episode(int number) const
{
    return m_episodes.value(number);
}

void Season::addEpisode(Episode *episode)
{
    Q_ASSERT(!m_episodes.contains(episode->number()));

    episode->setSeason(this);
    m_episodes.insert(episode->number(), episode);
}

void Season::removeEpisode(Episode *episode)
{
    Q_ASSERT(m_episodes.contains(episode->number()));

    episode->setSeason(nullptr);
    m_episodes.remove(episode->number());
}

void Season::setEpisodes(const QList<Episode *> &episodes)
{
    m_episodes.clear();

    foreach(Episode *episode, episodes) {
        addEpisode(episode);
    }
}
