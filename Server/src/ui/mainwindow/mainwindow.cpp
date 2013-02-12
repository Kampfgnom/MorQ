#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "ui/preferences/preferenceswindow.h"

#include <QLabel>
#include <QSettings>

MainWindow *MainWindow::s_instance = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_preferencesWindow(nullptr),
    m_movieBusy(QLatin1String(":animations/busy_small"))
{
    ui->setupUi(this);

    QButtonGroup *sideBarButtonGroup = new QButtonGroup(ui->sideBar);
    sideBarButtonGroup->addButton(ui->downloadsButton);
    ui->downloadsButton->setDefaultAction(ui->actionDownloads);

    QActionGroup *viewActionGroup = new QActionGroup(this);
    viewActionGroup->addAction(ui->actionDownloads);

    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    statusBar()->addWidget(spacer, 1);

    m_busyLabel = new QLabel(this);
    m_busyLabel->setMovie(&m_movieBusy);
    statusBar()->addWidget(m_busyLabel);
    m_busyLabel->setVisible(false);

    m_statusMessageLabel = new QLabel(this);
    statusBar()->addWidget(m_statusMessageLabel);
}

MainWindow *MainWindow::instance()
{
    if(!s_instance) {
        s_instance = new MainWindow;
    }
    return s_instance;
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::isBusy() const
{
    return m_busy;
}

void MainWindow::setBusy(bool busy)
{
    if(busy == m_busy)
        return;

    m_busy = busy;
    if(busy) {
        m_movieBusy.start();
        m_busyLabel->setVisible(true);
    }
    else {
        m_movieBusy.stop();
        m_busyLabel->setVisible(false);
    }

}

void MainWindow::setStatusMessage(const QString &message)
{
    m_statusMessageLabel->setText(message);
}

void MainWindow::on_actionDownloads_triggered()
{
    ui->actionDownloads->setChecked(true);
    ui->downloadsButton->setChecked(true);
    ui->centralStackedWidget->setCurrentWidget(ui->downloadsPage);

    // TODO: disable other actions/buttons
}

void MainWindow::on_actionPreferences_triggered()
{
    if(!m_preferencesWindow) {
        m_preferencesWindow = new PreferencesWindow;
        connect(m_preferencesWindow, &QObject::destroyed, [&]() {
            m_preferencesWindow = nullptr;
        });
    }

    m_preferencesWindow->raise();
    m_preferencesWindow->activateWindow();
    m_preferencesWindow->show();
}
