#include "controller.h"

#include "downloadcontroller.h"
#include "plugincontroller.h"
#include "linkscontroller.h"

#include <QPersistence/databaseschema.h>

#include <QObject>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>
#include <QNetworkAccessManager>

QPersistence::PersistentDataAccessObject<Download> *Controller::s_downloadsDao = nullptr;
QPersistence::PersistentDataAccessObject<DownloadPackage> *Controller::s_downloadPackagesDao = nullptr;

static QObject GUARD;

bool Controller::initialize()
{
    if(!initlializeDatabase())
        return false;

    QDataSuite::registerMetaObject<Download>();
    s_downloadsDao = new QPersistence::PersistentDataAccessObject<Download>(QSqlDatabase::database(), &GUARD);
    QPersistence::registerPersistentDataAccessObject<Download>(s_downloadsDao);

    QDataSuite::registerMetaObject<DownloadPackage>();
    s_downloadPackagesDao = new QPersistence::PersistentDataAccessObject<DownloadPackage>(QSqlDatabase::database(), &GUARD);
    QPersistence::registerPersistentDataAccessObject<DownloadPackage>(s_downloadPackagesDao);

    QPersistence::DatabaseSchema databaseSchema;
    databaseSchema.createCleanSchema();

    plugins();
    links();
    downloads();

    return true;
}

DownloadController *Controller::downloads()
{
    static DownloadController *controller = nullptr;

    if(!controller)
        controller = new DownloadController(&GUARD);

    return controller;
}

QPersistence::PersistentDataAccessObject<Download> *Controller::downloadsDao()
{
    return s_downloadsDao;
}

QPersistence::PersistentDataAccessObject<DownloadPackage> *Controller::downloadPackagesDao()
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

bool Controller::initlializeDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.sqlite");
    if(!db.open()) {
        qCritical() << db.lastError();
        return false;
    }

    return true;
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
