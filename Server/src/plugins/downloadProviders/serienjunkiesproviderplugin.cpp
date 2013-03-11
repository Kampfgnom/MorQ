#include "serienjunkiesproviderplugin.h"

#include "controller/controller.h"

#include "model/series.h"
#include "model/season.h"
#include "model/episode.h"
#include "model/videodownloadlink.h"

#include <qserienjunkies.h>

#include <QDebug>

SerienjunkiesProviderPlugin::SerienjunkiesProviderPlugin(QObject *parent) :
    DownloadProviderPlugin("serienjunkies.org", parent),
    m_latestReply(nullptr)
{
    QSerienJunkies::setNetworkAccessManager(Controller::networkAccessManager());
}

void SerienjunkiesProviderPlugin::searchSeries(const QString &seriesTitle)
{
    if(m_latestReply) {
        delete m_latestReply;
        m_latestReply = nullptr;
    }

    QSerienJunkiesReply *reply = QSerienJunkies::searchSeries(seriesTitle);
    m_latestReply = reply;

    connect(reply, &QSerienJunkiesReply::finished, [=]() {
        QList<DownloadProviderPlugin::SeriesData> result;
        foreach(QSerienJunkiesReply::Series series, reply->series()) {
            DownloadProviderPlugin::SeriesData s;
            s.title = series.name;
            s.url = series.url;
            result.append(s);
        }

        reply->deleteLater();
        if(reply == m_latestReply)
            m_latestReply = nullptr;
        emit this->foundSeries(result);
    });

    connect(reply, &QSerienJunkiesReply::error, [=]() {
        setErrorString(reply->errorString());

        reply->deleteLater();
        if(reply == m_latestReply)
            m_latestReply = nullptr;
        emit this->error();
    });
}

bool SerienjunkiesProviderPlugin::canHandleSeries(Series *series) const
{
    return !series->serienJunkiesUrl().isEmpty()
            && series->serienJunkiesUrl().host().startsWith("serienjunkies");
}

void SerienjunkiesProviderPlugin::findMissingEpisodes(Series *series) const
{
    SerienjunkiesSearchHandler *handler = new SerienjunkiesSearchHandler(const_cast<SerienjunkiesProviderPlugin *>(this));
    handler->findMissingEpisodes(series);
}


SerienjunkiesSearchHandler::SerienjunkiesSearchHandler(QObject *parent) :
    QObject(parent),
    m_series(nullptr),
    m_seasonsNotFinishedCount(0)
{
}

void SerienjunkiesSearchHandler::findMissingEpisodes(Series *series)
{
    m_series = series;

    QSerienJunkiesReply *reply = QSerienJunkies::searchSeasons(series->serienJunkiesUrl());

    connect(reply, &QSerienJunkiesReply::finished,
            this, &SerienjunkiesSearchHandler::searchSeasonsFinished);

    connect(reply, &QSerienJunkiesReply::error, [=]() {
        reply->deleteLater();
        this->deleteLater();
    });

}

void SerienjunkiesSearchHandler::searchSeasonsFinished()
{
    QSerienJunkiesReply *reply = static_cast<QSerienJunkiesReply *>(sender());

    foreach(QSerienJunkiesReply::Season se, reply->seasons()) {
        int number = QSerienJunkies::seasonNumberFromTitle(se.title);
        Season *season = m_series->season(number);

        if(!season) {
            season = Controller::seasonsDao()->create();
            season->setNumber(number);
            m_series->addSeason(season);
            Controller::seasonsDao()->insert(season);
        }

        season->setSerienJunkiesTitle(se.title);
        season->setSerienJunkiesUrl(se.url);
        Controller::seasonsDao()->update(season);

        QSerienJunkiesReply *episodeReply = QSerienJunkies::searchDownloads(se.url);
        connect(episodeReply, &QSerienJunkiesReply::finished,
                this, &SerienjunkiesSearchHandler::searchEpisodesFinished);

        ++m_seasonsNotFinishedCount;

        connect(episodeReply, &QSerienJunkiesReply::error, [=]() {
            --m_seasonsNotFinishedCount;
            if(m_seasonsNotFinishedCount == 0)
                this->deleteLater();

            episodeReply->deleteLater();
        });
    }

    reply->deleteLater();
}

void SerienjunkiesSearchHandler::searchEpisodesFinished()
{
    QSerienJunkiesReply *reply = static_cast<QSerienJunkiesReply *>(sender());

    foreach(QSerienJunkiesReply::Format format, reply->formats()) {
        foreach(QSerienJunkiesReply::DownloadLink link, reply->downloadLinks(format, format.mirrors)) {
            int snumber = QSerienJunkies::seasonNumberFromName(link.name);
            int enumber = QSerienJunkies::episodeNumberFromName(link.name);

            Season *season = m_series->season(snumber);
            if(!season) {
                continue;
            }

            Episode *episode = season->episode(enumber);

            if(!episode) {
                episode = Controller::episodesDao()->create();
                episode->setNumber(enumber);
                episode->setSerienJunkiesTitle(link.name);
                season->addEpisode(episode);
                Controller::episodesDao()->insert(episode);
            }
        }
    }

    reply->deleteLater();

    --m_seasonsNotFinishedCount;
    if(m_seasonsNotFinishedCount == 0)
        this->deleteLater();
}
