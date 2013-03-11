#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QDataSuite/cacheddataaccessobject.h>

class DownloadController;
class PluginController;
class QNetworkAccessManager;
class Download;
class DownloadPackage;
class LinksController;
class ExtractionController;
class SeriesController;
class Series;
class Season;
class Episode;
class VideoDownloadLink;

class Controller
{
public:
    static bool initialize();

    static DownloadController *downloads();
    static PluginController *plugins();
    static LinksController *links();
    static ExtractionController *extractor();
    static SeriesController *series();

    static QDataSuite::CachedDataAccessObject<Download> *downloadsDao();
    static QDataSuite::CachedDataAccessObject<DownloadPackage> *downloadPackagesDao();
    static QDataSuite::CachedDataAccessObject<Series> *seriesDao();
    static QDataSuite::CachedDataAccessObject<Season> *seasonsDao();
    static QDataSuite::CachedDataAccessObject<Episode> *episodesDao();
    static QDataSuite::CachedDataAccessObject<VideoDownloadLink> *videoDownloadLinksDao();

    static QNetworkAccessManager *networkAccessManager();

private:
    static QDataSuite::CachedDataAccessObject<Download> *s_downloadsDao;
    static QDataSuite::CachedDataAccessObject<DownloadPackage> *s_downloadPackagesDao;
    static QDataSuite::CachedDataAccessObject<Series> *s_seriesDao;
    static QDataSuite::CachedDataAccessObject<Season> *s_seasonDao;
    static QDataSuite::CachedDataAccessObject<Episode> *s_episodesDao;
    static QDataSuite::CachedDataAccessObject<VideoDownloadLink> *s_videoDownloadLinksDao;
};

#endif // CONTROLLER_H
