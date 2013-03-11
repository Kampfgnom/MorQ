#include "controller.h"

#include "downloadcontroller.h"
#include "plugincontroller.h"
#include "linkscontroller.h"
#include "extractioncontroller.h"
#include "seriescontroller.h"

#include "model/download.h"
#include "model/downloadpackage.h"
#include "model/series.h"
#include "model/season.h"
#include "model/episode.h"
#include "model/videodownloadlink.h"

#include <QPersistence/databaseschema.h>
#include <QPersistence/persistentdataaccessobject.h>

#include <QObject>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>
#include <QNetworkAccessManager>

QDataSuite::CachedDataAccessObject<Download> *Controller::s_downloadsDao = nullptr;
QDataSuite::CachedDataAccessObject<DownloadPackage> *Controller::s_downloadPackagesDao = nullptr;
QDataSuite::CachedDataAccessObject<Series> *Controller::s_seriesDao = nullptr;
QDataSuite::CachedDataAccessObject<Season> *Controller::s_seasonDao = nullptr;
QDataSuite::CachedDataAccessObject<Episode> *Controller::s_episodesDao = nullptr;
QDataSuite::CachedDataAccessObject<VideoDownloadLink> *Controller::s_videoDownloadLinksDao = nullptr;

static QObject GUARD;

bool Controller::initialize()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.sqlite");
    if(!db.open()) {
        qCritical() << db.lastError();
        return false;
    }

    // Download
    QDataSuite::registerMetaObject<Download>();
    QPersistence::PersistentDataAccessObject<Download> *downloadsDao =
            new QPersistence::PersistentDataAccessObject<Download>(QSqlDatabase::database(), &GUARD);

    s_downloadsDao = new QDataSuite::CachedDataAccessObject<Download>(downloadsDao, &GUARD);
    QDataSuite::registerDataAccessObject<Download>(s_downloadsDao, db.connectionName());


    // DownloadPackage
    QDataSuite::registerMetaObject<DownloadPackage>();
    QPersistence::PersistentDataAccessObject<DownloadPackage> *downloadPackagesDao =
            new QPersistence::PersistentDataAccessObject<DownloadPackage>(QSqlDatabase::database(), &GUARD);

    s_downloadPackagesDao = new QDataSuite::CachedDataAccessObject<DownloadPackage>(downloadPackagesDao, &GUARD);
    QDataSuite::registerDataAccessObject<DownloadPackage>(s_downloadPackagesDao, db.connectionName());


    // Series
    QDataSuite::registerMetaObject<Series>();
    QPersistence::PersistentDataAccessObject<Series> *seriesDao =
            new QPersistence::PersistentDataAccessObject<Series>(QSqlDatabase::database(), &GUARD);

    s_seriesDao = new QDataSuite::CachedDataAccessObject<Series>(seriesDao, &GUARD);
    QDataSuite::registerDataAccessObject<Series>(s_seriesDao, db.connectionName());


    // Season
    QDataSuite::registerMetaObject<Season>();
    QPersistence::PersistentDataAccessObject<Season> *seasonDao =
            new QPersistence::PersistentDataAccessObject<Season>(QSqlDatabase::database(), &GUARD);

    s_seasonDao = new QDataSuite::CachedDataAccessObject<Season>(seasonDao, &GUARD);
    QDataSuite::registerDataAccessObject<Season>(s_seasonDao, db.connectionName());


    // Episode
    QDataSuite::registerMetaObject<Episode>();
    QPersistence::PersistentDataAccessObject<Episode> *episodesDao =
            new QPersistence::PersistentDataAccessObject<Episode>(QSqlDatabase::database(), &GUARD);

    s_episodesDao = new QDataSuite::CachedDataAccessObject<Episode>(episodesDao, &GUARD);
    QDataSuite::registerDataAccessObject<Episode>(s_episodesDao, db.connectionName());


    // VideoDownloadLink
    QDataSuite::registerMetaObject<VideoDownloadLink>();
    QPersistence::PersistentDataAccessObject<VideoDownloadLink> *downloadLinksDao =
            new QPersistence::PersistentDataAccessObject<VideoDownloadLink>(QSqlDatabase::database(), &GUARD);

    s_videoDownloadLinksDao = new QDataSuite::CachedDataAccessObject<VideoDownloadLink>(downloadLinksDao, &GUARD);
    QDataSuite::registerDataAccessObject<VideoDownloadLink>(s_videoDownloadLinksDao, db.connectionName());


    // Adjust database
    QPersistence::DatabaseSchema databaseSchema;
    //    databaseSchema.createCleanSchema();
    databaseSchema.adjustSchema();


    // Call these methods, to once initialize each controller
    plugins();
    links();
    downloads();
    extractor();
    series();

    return true;
}

DownloadController *Controller::downloads()
{
    static DownloadController *controller = nullptr;

    if(!controller)
        controller = new DownloadController(&GUARD);

    return controller;
}

QDataSuite::CachedDataAccessObject<Download> *Controller::downloadsDao()
{
    return s_downloadsDao;
}

QDataSuite::CachedDataAccessObject<DownloadPackage> *Controller::downloadPackagesDao()
{
    return s_downloadPackagesDao;
}

QDataSuite::CachedDataAccessObject<Series> *Controller::seriesDao()
{
    return s_seriesDao;
}

QDataSuite::CachedDataAccessObject<Season> *Controller::seasonsDao()
{
    return s_seasonDao;
}

QDataSuite::CachedDataAccessObject<Episode> *Controller::episodesDao()
{
    return s_episodesDao;
}

QDataSuite::CachedDataAccessObject<VideoDownloadLink> *Controller::videoDownloadLinksDao()
{
    return s_videoDownloadLinksDao;
}

PluginController *Controller::plugins()
{
    static PluginController *controller = nullptr;

    if(!controller)
        controller = new PluginController(&GUARD);

    return controller;
}

LinksController *Controller::links()
{
    static LinksController *controller = nullptr;

    if(!controller)
        controller = new LinksController(&GUARD);

    return controller;
}

ExtractionController *Controller::extractor()
{
    static ExtractionController *controller = nullptr;

    if(!controller)
        controller = new ExtractionController(&GUARD);

    return controller;
}

SeriesController *Controller::series()
{
    static SeriesController *controller = nullptr;

    if(!controller)
        controller = new SeriesController(&GUARD);

    return controller;
}

QNetworkAccessManager *Controller::networkAccessManager()
{
    static QNetworkAccessManager *nam = nullptr;

    if(!nam)
        nam = new QNetworkAccessManager;

    static QObject guard;
    QObject::connect(&guard, &QObject::destroyed, [&]() {
        nam->deleteLater();
    });

    return nam;
}


