#ifndef DOWNLOADCONTROLLER_H
#define DOWNLOADCONTROLLER_H

#include <QObject>

#include "controller.h"
#include "model/downloadsitemmodel.h"

#include <download.h>
#include <downloadpackage.h>

#include <QClipboard>
#include <QTimer>

class Download;
class DownloadPackage;
class QDownload;
class QShareHoster;

class DownloadController : public QObject
{
    Q_OBJECT
public:
    void addDownload(const QUrl &url);

    DownloadsItemModel *model() const;

private slots:

private:
    friend class Controller;
    explicit DownloadController(QObject *parent = 0);
    DownloadsItemModel *m_model;

    Q_DISABLE_COPY(DownloadController)
};

#endif // DOWNLOADCONTROLLER_H
