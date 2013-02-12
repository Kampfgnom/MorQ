#include "controller.h"

#include "downloadcontroller.h"

#include <QObject>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>

bool Controller::initialize()
{
    if(!initlializeDatabase())
        return false;

    downloads();
//    databaseSchema.createCleanSchema();

    return true;
}

DownloadController *Controller::downloads()
{
    static QObject guard;
    static DownloadController *downloadController = nullptr;

    if(!downloadController)
        downloadController = new DownloadController(&guard);

    return downloadController;
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
