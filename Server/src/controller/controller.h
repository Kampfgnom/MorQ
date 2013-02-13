#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QPersistence/persistentdataaccessobject.h>

class DownloadController;
class PluginController;
class QNetworkAccessManager;
class Download;
class DownloadPackage;
class LinksController;

class Controller
{
public:
    static bool initialize();

    static DownloadController *downloads();
    static PluginController *plugins();
    static LinksController *links();

    static QPersistence::PersistentDataAccessObject<Download> *downloadsDao();
    static QPersistence::PersistentDataAccessObject<DownloadPackage> *downloadPackagesDao();

    static QNetworkAccessManager *networkAccessManager();

private:
    static bool initlializeDatabase();

    static QPersistence::PersistentDataAccessObject<Download> *s_downloadsDao;
    static QPersistence::PersistentDataAccessObject<DownloadPackage> *s_downloadPackagesDao;
};

#endif // CONTROLLER_H
