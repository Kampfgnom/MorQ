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

class Controller
{
public:
    static bool initialize();

    static DownloadController *downloads();
    static PluginController *plugins();
    static LinksController *links();
    static ExtractionController *extractor();

    static QDataSuite::CachedDataAccessObject<Download> *downloadsDao();
    static QDataSuite::CachedDataAccessObject<DownloadPackage> *downloadPackagesDao();

    static QNetworkAccessManager *networkAccessManager();

private:
    static QDataSuite::CachedDataAccessObject<Download> *s_downloadsDao;
    static QDataSuite::CachedDataAccessObject<DownloadPackage> *s_downloadPackagesDao;
};

#endif // CONTROLLER_H
