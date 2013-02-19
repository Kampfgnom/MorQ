#include "extractioncontroller.h"

#include "controller.h"
#include "preferences.h"

#include <download.h>
#include <downloadpackage.h>

#include <QuunRar.h>
#include <QuunRarJob.h>

#include <QDir>
#include <QElapsedTimer>

ExtractionController::ExtractionController(QObject *parent) :
    QObject(parent)
{
    foreach(DownloadPackage *package, Controller::downloadPackagesDao()->readAll()) {
        connect(package, &DownloadPackage::downloadFinished,
                this, &ExtractionController::extractFinishedPackage);
    }

    connect(Controller::downloadPackagesDao(), &QDataSuite::AbstractDataAccessObject::objectInserted, [=](QObject *obj) {
        DownloadPackage *package = qobject_cast<DownloadPackage *>(obj);
        if(!package)
            return;

        connect(package, &DownloadPackage::downloadFinished,
                this, &ExtractionController::extractFinishedPackage);
    });
}

ExtractionController::~ExtractionController()
{
    foreach(QuunRarJob *job, m_jobs) {
        job->quit();
    }

    foreach(QElapsedTimer *timer, m_timers.values()) {
        delete timer;
    }
}

void ExtractionController::extractPackage(DownloadPackage *package)
{
    if(package->downloads().isEmpty())
        return;

//    if(!package->isDownloadFinished())
//        return;

    QList<Download *> dls = package->downloads();
    qSort(dls.begin(), dls.end(), [](Download *dl1, Download *dl2) {
        return dl1->fileName() < dl2->fileName();
    });

    Download *dl = dls.first();

    if(dl->destinationFolder().isEmpty())
        dl->setDestinationFolder(Preferences::downloadFolder());

    QuunRar *rar = new QuunRar(dl->destinationFolder() + QDir::separator() + dl->fileName(), this);
    rar->setDestinationDir(Preferences::extractFolder());
    rar->setPassword("serienjunkies.org");
    if(!rar->open()) {
        package->setMessage(QLatin1String("Extraction failed: ") + rar->errorString());
        Controller::downloadPackagesDao()->update(package);
        return;
    }

    QuunRarJob *job = rar->extract();
    package->setExtractedFileSize(rar->totalUnpackedSize());

    QElapsedTimer *timer = new QElapsedTimer;
    m_timers.insert(job, timer);
    timer->start();

    m_jobs.append(job);
    m_runningExtractions.insert(job, package);
    m_currentExtractingDownloads.insert(package, dl);
    Controller::downloadPackagesDao()->update(package);

    // Must not use lambdas, because the signals come from a different thread!
    connect(job, &QuunRarJob::dataProcessed,
            this, &ExtractionController::bytesProcessed);

    connect(job, &QuunRarJob::volumeChanged,
            this, &ExtractionController::volumeChanged);

    connect(job, &QuunRarJob::finished,
            this, &ExtractionController::finished);

    connect(job, &QuunRarJob::currentFileChanged,
            this, &ExtractionController::currentFileChanged);

    connect(job, &QuunRarJob::error,
            this, &ExtractionController::error);

    connect(dl, &QObject::destroyed,
            this, &ExtractionController::downloadDestroyed);

    connect(package, &QObject::destroyed, [=]() {
        quitAndRemoveJob(job);
    });
}

void ExtractionController::extractFinishedPackage()
{
    DownloadPackage *package = qobject_cast<DownloadPackage *>(sender());
    if(!package)
        return;

    extractPackage(package);
}

void ExtractionController::bytesProcessed()
{
    QuunRarJob *job = static_cast<QuunRarJob *>(sender());
    QElapsedTimer *timer = m_timers.value(job);
    if(!timer)
        return;

    if(!timer->hasExpired(1000))
        return;
    timer->restart();

    DownloadPackage *package = m_runningExtractions.value(job);
    if(!package) {
        quitAndRemoveJob(job);
        return;
    }

    package->setBytesExtracted(job->processedData());
    Controller::downloadPackagesDao()->update(package);

    Download *dl = m_currentExtractingDownloads.value(package);
    if(dl) {
        dl->setExtracting(true);
        Controller::downloadsDao()->update(dl);
    }
}

void ExtractionController::volumeChanged(const QString &volumeName)
{
    QuunRarJob *job = static_cast<QuunRarJob *>(sender());

    DownloadPackage *package = m_runningExtractions.value(job);
    if(!package) {
        quitAndRemoveJob(job);
        return;
    }

    Download *dl = m_currentExtractingDownloads.value(package);
    if(dl) {
        disconnect(dl, 0, this, 0);
        dl->setExtracting(false);
        Controller::downloadsDao()->update(dl);
    }

    QFileInfo info(volumeName);
    QString fileName = info.fileName();

    foreach(dl, package->downloads()) {
        if(dl->fileName() == fileName) {
            connect(dl, &QObject::destroyed,
                    this, &ExtractionController::downloadDestroyed);

            dl->setExtracting(true);
            m_currentExtractingDownloads.remove(package);
            m_currentExtractingDownloads.insert(package, dl);
            Controller::downloadsDao()->update(dl);
            break;
        }
    }
}

void ExtractionController::finished()
{
    QuunRarJob *job = static_cast<QuunRarJob *>(sender());

    DownloadPackage *package = m_runningExtractions.value(job);
    if(!package) {
        quitAndRemoveJob(job);
        return;
    }

    Download *dl = m_currentExtractingDownloads.value(package);
    if(dl) {
        dl->setExtracting(false);
        m_currentExtractingDownloads.remove(package);
        Controller::downloadsDao()->update(dl);
    }

    package->setBytesExtracted(job->processedData());
    package->setMessage(tr("Extract OK"));
    Controller::downloadPackagesDao()->update(package);

    quitAndRemoveJob(job);
}

void ExtractionController::currentFileChanged(const QString &fileName)
{
    QuunRarJob *job = static_cast<QuunRarJob *>(sender());

    DownloadPackage *package = m_runningExtractions.value(job);
    if(!package) {
        quitAndRemoveJob(job);
        return;
    }

    package->setMessage(tr("Extracting %1").arg(fileName));
    Controller::downloadPackagesDao()->update(package);
}

void ExtractionController::error()
{
    QuunRarJob *job = static_cast<QuunRarJob *>(sender());

    DownloadPackage *package = m_runningExtractions.value(job);
    if(!package) {
        quitAndRemoveJob(job);
        return;
    }

    package->setMessage(QLatin1String("Extraction failed: ") + job->archive()->errorString());
    quitAndRemoveJob(job);
}

void ExtractionController::downloadDestroyed()
{
    Download *dl = static_cast<Download *>(sender());
    m_currentExtractingDownloads.remove(dl->package());
}

void ExtractionController::quitAndRemoveJob(QuunRarJob *job)
{
    QElapsedTimer *timer = m_timers.value(job);
    if(timer) {
        delete timer;
        m_timers.remove(job);
    }

    DownloadPackage *package = m_runningExtractions.value(job);
    if(package) {
        if(m_currentExtractingDownloads.contains(package)) {
            m_currentExtractingDownloads.remove(package);
        }
        m_runningExtractions.remove(job);
    }

    if(m_jobs.contains(job)) {
        if(job->isRunning())
            job->quit();

//        job->archive()->deleteLater();
        m_jobs.removeAll(job);
    }
}
