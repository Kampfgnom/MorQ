#ifndef EXTRACTIONCONTROLLER_H
#define EXTRACTIONCONTROLLER_H

#include <QObject>

#include <QHash>

class DownloadPackage;
class QElapsedTimer;
class Download;
class QuunRarJob;

class ExtractionController : public QObject
{
    Q_OBJECT
public:
    explicit ExtractionController(QObject *parent = 0);
    ~ExtractionController();
    
    void extractPackage(DownloadPackage *package);

private slots:
    void extractFinishedPackage();

    void bytesProcessed();
    void volumeChanged(const QString &volumnName);
    void finished();
    void currentFileChanged(const QString &fileName);
    void error();

    void downloadDestroyed();

private:
    QHash<QuunRarJob *, QElapsedTimer *> m_timers;
    QHash<DownloadPackage *, Download *> m_currentExtractingDownloads;
    QHash<QuunRarJob *, DownloadPackage *> m_runningExtractions;
    QList<QuunRarJob *> m_jobs;

    void quitAndRemoveJob(QuunRarJob *job);
};

#endif // EXTRACTIONCONTROLLER_H
