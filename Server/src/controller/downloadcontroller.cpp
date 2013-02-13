#include "downloadcontroller.h"

DownloadController::DownloadController(QObject *parent) :
    QObject(parent)
{
    m_model = new DownloadsItemModel(Controller::downloadPackagesDao(),
                                     Controller::downloadsDao(),
                                     this);
}

DownloadsItemModel *DownloadController::model() const
{
    return m_model;
}
