#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMovie>

namespace Ui {
class MainWindow;
}

class QLabel;
class PreferencesWindow;
class DownloadsItemModel;
class SeriesListModel;
class SeasonsListModel;
class EpisodesListModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    static MainWindow *instance();
    ~MainWindow();

private slots:
    void on_actionDownloads_triggered();
    void on_actionTV_Shows_triggered();

    void on_actionPreferences_triggered();
    void on_actionDownload_Preferences_triggered();
    void on_actionPremiumizeMe_Preferences_triggered();

    void on_actionStart_triggered();
    void on_actionDeleteDownload_triggered();
    void on_actionResetDownload_triggered();
    void on_actionStopDownloads_triggered();
    void on_actionExtract_triggered();

    void enableActionsAccordingToDownloadSelection();
    void enableActionsAccordingToDownloadStatus();

    void on_actionAdd_show_triggered();

    void showSeasonsForSelectedSeries();
    void showEpisodesForSelectedSeason();

private:
    static MainWindow *s_instance;
    explicit MainWindow(QWidget *parent = 0);

    Ui::MainWindow *ui;

    PreferencesWindow *m_preferencesWindow;
    DownloadsItemModel *m_downloadsModel;
    SeriesListModel *m_seriesModel;
    SeasonsListModel *m_seasonsModel;
    EpisodesListModel *m_episodesModel;
};

#endif // MAINWINDOW_H
