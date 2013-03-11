#ifndef SERIENJUNKIESPROVIDERPLUGIN_H
#define SERIENJUNKIESPROVIDERPLUGIN_H

#include "downloadproviderplugin.h"

class QSerienJunkiesReply;

class SerienjunkiesProviderPlugin : public DownloadProviderPlugin
{
    Q_OBJECT
public:
    SerienjunkiesProviderPlugin(QObject *parent = 0);

    void searchSeries(const QString &seriesName);
    bool canHandleSeries(Series *series) const;
    void findMissingEpisodes(Series *series) const;

private:
    QSerienJunkiesReply *m_latestReply;
};

class SerienjunkiesSearchHandler : public QObject
{
    Q_OBJECT
public:
    SerienjunkiesSearchHandler(QObject *parent);
    void findMissingEpisodes(Series *series);

private slots:
    void searchSeasonsFinished();
    void searchEpisodesFinished();

private:
    Series *m_series;
    int m_seasonsNotFinishedCount;
};

#endif // SERIENJUNKIESPROVIDERPLUGIN_H
