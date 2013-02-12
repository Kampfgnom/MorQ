#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMovie>

namespace Ui {
class MainWindow;
}

class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    static MainWindow *instance();

    ~MainWindow();
    
    bool isBusy() const;
    void setBusy(bool busy);
    void setStatusMessage(const QString &message);

private slots:
    void on_actionDownloads_triggered();

private:
    static MainWindow *s_instance;
    explicit MainWindow(QWidget *parent = 0);

    Ui::MainWindow *ui;

    bool m_busy;
    QLabel *m_busyLabel;
    QMovie m_movieBusy;
    QLabel *m_statusMessageLabel;
};

#endif // MAINWINDOW_H
