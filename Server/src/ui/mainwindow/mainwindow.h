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

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    static MainWindow *instance();

    ~MainWindow();
    
    bool isBusy() const;
    void setBusy(bool busy);
    void setStatusMessage(const QString &message);

    void closeEvent(QCloseEvent *);

private slots:
    void on_actionDownloads_triggered();

    void on_actionPreferences_triggered();

    void on_actionDownload_Preferences_triggered();

    void on_actionPremiumizeMe_Preferences_triggered();

    void on_actionStart_triggered();

    void on_actionDeleteDownload_triggered();

    void on_actionResetDownload_triggered();

private:
    static MainWindow *s_instance;
    explicit MainWindow(QWidget *parent = 0);

    Ui::MainWindow *ui;

    PreferencesWindow *m_preferencesWindow;
    DownloadsItemModel *m_downloadsModel;

    bool m_busy;
    QLabel *m_busyLabel;
    QMovie m_movieBusy;
    QLabel *m_statusMessageLabel;
};

#endif // MAINWINDOW_H
