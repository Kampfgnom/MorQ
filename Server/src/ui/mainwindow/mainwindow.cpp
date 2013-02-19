#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "downloadsitemdelegate.h"

#include "ui/preferences/preferenceswindow.h"
#include "controller/controller.h"
#include "model/downloadsitemmodel.h"
#include "controller/downloadcontroller.h"
#include "controller/extractioncontroller.h"

#include <download.h>
#include <downloadpackage.h>

#include <QLabel>
#include <QSettings>
#include <QDebug>
#include <QCloseEvent>
#include <QMessageBox>

static const QString WINDOWGEOMETRY("ui/mainwindow/geometry");
static const QString WINDOWSTATE("ui/mainwindow/state");
static const QString DOWNLOADSHEADERSTATE("ui/mainwindow/downloads/headerstate");

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

    m_downloadsModel = new DownloadsItemModel(this);

    ui->treeViewDownloads->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->treeViewDownloads->setModel(m_downloadsModel);
    ui->treeViewDownloads->setItemDelegate(new DownloadsItemDelegate(this));
    ui->treeViewDownloads->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->treeViewDownloads->addAction(ui->actionDeleteDownload);
    ui->treeViewDownloads->addAction(ui->actionResetDownload);
    ui->treeViewDownloads->addAction(ui->actionExtract);
    ui->treeViewDownloads->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(ui->treeViewDownloads->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::onDownloadsSelectionChanged);

    QSettings settings;
    restoreGeometry(settings.value(WINDOWGEOMETRY, "").toByteArray());
    restoreState(settings.value(WINDOWSTATE, "").toByteArray());
    ui->treeViewDownloads->header()->restoreState(settings.value(DOWNLOADSHEADERSTATE, "").toByteArray());

    connect(Controller::downloads(), &DownloadController::statusChanged,
            this, &MainWindow::onDownloadControllerStatusChanged);

    onDownloadsSelectionChanged();
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
    QSettings settings;
    settings.setValue(WINDOWGEOMETRY, saveGeometry());
    settings.setValue(WINDOWSTATE, saveState());
    settings.setValue(DOWNLOADSHEADERSTATE, ui->treeViewDownloads->header()->saveState());

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

void MainWindow::closeEvent(QCloseEvent *e)
{
    QMainWindow::closeEvent(e);
//    setVisible(false);
//    e->setAccepted(true);
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

void MainWindow::on_actionDownload_Preferences_triggered()
{
    on_actionPreferences_triggered();
    if(!m_preferencesWindow)
        return;

    m_preferencesWindow->setCurrentPage(PreferencesWindow::DownloadsPage);
}

void MainWindow::on_actionPremiumizeMe_Preferences_triggered()
{
    on_actionPreferences_triggered();
    if(!m_preferencesWindow)
        return;

    m_preferencesWindow->setCurrentPage(PreferencesWindow::PremiumizeMePage);
}

void MainWindow::on_actionStart_triggered()
{
    Controller::downloads()->startDownloads();
}

void MainWindow::on_actionStopDownloads_triggered()
{
    Controller::downloads()->stopDownloads();
}

void MainWindow::onDownloadControllerStatusChanged()
{
    bool running = Controller::downloads()->isDownloadRunning();

    ui->actionStart->setEnabled(!running);
    ui->actionStopDownloads->setEnabled(running);
}

void MainWindow::on_actionDeleteDownload_triggered()
{
    // TODO: confirmation
//    QMessageBox confirmDialog(this);
//    confirmDialog.setText(tr("Do you really want to remove the selected downloads?"));
//    confirmDialog.setWindowTitle(tr("Confirm remove"));
//    confirmDialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
//    confirmDialog.setDefaultButton(QMessageBox::Yes);


    QModelIndexList list = ui->treeViewDownloads->selectionModel()->selectedRows();

    QList<Download *> selectedDownloads;
    QList<DownloadPackage *> selectedPackages;

    foreach(QModelIndex index, list) {
        if(index.parent().isValid()) {
            selectedDownloads.append(m_downloadsModel->downloadByIndex(index));
        }
        else if(!index.parent().isValid()) {
            selectedPackages.append(m_downloadsModel->packageByIndex(index));
        }
    }

//    QString informativeText(tr("Packages:\n"));
//    foreach(DownloadPackage *package, selectedPackages) {
//        informativeText += package->name() + "\n";
//    }
//    informativeText += tr("Downloads:\n");
//    foreach(Download *dl, selectedDownloads) {
//        informativeText += dl->fileName() + "\n";
//    }
//    confirmDialog.setInformativeText(informativeText);
//    int result = confirmDialog.exec();
//    if(result != QMessageBox::Yes)
//        return;

    foreach(Download *dl, selectedDownloads) {
        if(dl)
            Controller::downloads()->removeDownload(dl);
    }

    foreach(DownloadPackage *package, selectedPackages) {
        if(package)
            Controller::downloads()->removePackage(package);
    }
}

void MainWindow::on_actionResetDownload_triggered()
{
    // TODO: confirmation
    QModelIndexList list = ui->treeViewDownloads->selectionModel()->selectedRows();

    QList<Download *> selectedDownloads;
    QList<DownloadPackage *> selectedPackages;

    foreach(QModelIndex index, list) {
        if(index.parent().isValid()) {
            selectedDownloads.append(m_downloadsModel->downloadByIndex(index));
        }
        else if(!index.parent().isValid()) {
            selectedPackages.append(m_downloadsModel->packageByIndex(index));
        }
    }

    foreach(Download *dl, selectedDownloads) {
        if(dl)
            Controller::downloads()->resetDownload(dl);
    }

    foreach(DownloadPackage *package, selectedPackages) {
        if(package)
            Controller::downloads()->resetPackage(package);
    }
}

void MainWindow::onDownloadsSelectionChanged()
{
    bool sel = !ui->treeViewDownloads->selectionModel()->selectedRows().isEmpty();

    ui->actionDeleteDownload->setEnabled(sel);
    ui->actionResetDownload->setEnabled(sel);
    ui->actionExtract->setEnabled(sel);
}

void MainWindow::on_actionExtract_triggered()
{
    QModelIndexList list = ui->treeViewDownloads->selectionModel()->selectedRows();
    QSet<DownloadPackage *> selectedPackages;

    foreach(QModelIndex index, list) {
        if(index.parent().isValid()) {
            selectedPackages.insert(m_downloadsModel->downloadByIndex(index)->package());
        }
        else if(!index.parent().isValid()) {
            selectedPackages.insert(m_downloadsModel->packageByIndex(index));
        }
    }

    foreach(DownloadPackage *package, selectedPackages) {
        if(package)
            Controller::extractor()->extractPackage(package);
    }
}
