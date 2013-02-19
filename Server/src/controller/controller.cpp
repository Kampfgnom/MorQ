#include "controller.h"

#include "downloadcontroller.h"
#include "plugincontroller.h"
#include "linkscontroller.h"
#include "extractioncontroller.h"

#include <download.h>
#include <downloadpackage.h>

#include <QPersistence/databaseschema.h>
#include <QPersistence/persistentdataaccessobject.h>

#include <QObject>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>
#include <QNetworkAccessManager>

QDataSuite::CachedDataAccessObject<Download> *Controller::s_downloadsDao = nullptr;
QDataSuite::CachedDataAccessObject<DownloadPackage> *Controller::s_downloadPackagesDao = nullptr;

static QObject GUARD;

bool Controller::initialize()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.sqlite");
    if(!db.open()) {
        qCritical() << db.lastError();
        return false;
    }

    QDataSuite::registerMetaObject<Download>();
    QPersistence::PersistentDataAccessObject<Download> *downloadsDao =
            new QPersistence::PersistentDataAccessObject<Download>(QSqlDatabase::database(), &GUARD);

    s_downloadsDao = new QDataSuite::CachedDataAccessObject<Download>(downloadsDao, &GUARD);
    QDataSuite::registerDataAccessObject<Download>(s_downloadsDao, db.connectionName());

    QDataSuite::registerMetaObject<DownloadPackage>();
    QPersistence::PersistentDataAccessObject<DownloadPackage> *downloadPackagesDao =
            new QPersistence::PersistentDataAccessObject<DownloadPackage>(QSqlDatabase::database(), &GUARD);

    s_downloadPackagesDao = new QDataSuite::CachedDataAccessObject<DownloadPackage>(downloadPackagesDao, &GUARD);
    QDataSuite::registerDataAccessObject<DownloadPackage>(s_downloadPackagesDao, db.connectionName());

    QPersistence::DatabaseSchema databaseSchema;
    //    databaseSchema.createCleanSchema();
    databaseSchema.adjustSchema();

    plugins();
    links();
    downloads();
    extractor();

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
