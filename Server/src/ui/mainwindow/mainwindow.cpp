#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "downloadsitemdelegate.h"
#include "serieslistitemdelegate.h"
#include "model/downloadsitemmodel.h"
#include "model/serieslistmodel.h"
#include "model/seasonslistmodel.h"
#include "model/episodeslistmodel.h"

#include "ui/dialogs/newserieswizard.h"
#include "ui/preferences/preferenceswindow.h"
#include "controller/controller.h"
#include "controller/downloadcontroller.h"
#include "controller/extractioncontroller.h"

#include "model/download.h"
#include "model/downloadpackage.h"

#include <QLabel>
#include <QSettings>
#include <QDebug>
#include <QCloseEvent>
#include <QMessageBox>

static const QString WINDOWGEOMETRY("ui/mainwindow/geometry");
static const QString WINDOWSTATE("ui/mainwindow/state");
static const QString DOWNLOADSHEADERSTATE("ui/mainwindow/downloads/headerstate");
static const QString SERIESPAGESPLITTERSTATE("ui/mainwindow/series/splitterstate");

MainWindow *MainWindow::s_instance = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_preferencesWindow(nullptr)
{
    ui->setupUi(this);

    // Init actions for sidebar and menu
    ui->buttonDownloads->setDefaultAction(ui->actionDownloads);
    ui->buttonSeries->setDefaultAction(ui->actionTV_Shows);

    QActionGroup *viewActionGroup = new QActionGroup(this);
    viewActionGroup->addAction(ui->actionDownloads);
    viewActionGroup->addAction(ui->actionTV_Shows);

    // Init downloads page
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
            this, &MainWindow::enableActionsAccordingToDownloadSelection);

    // Init series page
    m_seriesModel = new SeriesListModel(this);
    ui->treeViewSeries->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->treeViewSeries->setModel(m_seriesModel);
    ui->treeViewSeries->setItemDelegate(new SeriesListItemDelegate(this));

    m_seasonsModel = new SeasonsListModel(this);
    ui->treeViewSeasons->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->treeViewSeasons->setModel(m_seasonsModel);

    m_episodesModel = new EpisodesListModel(this);
    ui->treeViewEpisodes->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->treeViewEpisodes->setModel(m_episodesModel);

    connect(ui->treeViewSeries->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::showSeasonsForSelectedSeries);

    connect(ui->treeViewSeasons->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::showEpisodesForSelectedSeason);

    // Restore settings
    QSettings settings;
    restoreGeometry(settings.value(WINDOWGEOMETRY, "").toByteArray());
    restoreState(settings.value(WINDOWSTATE, "").toByteArray());
    ui->treeViewDownloads->header()->restoreState(settings.value(DOWNLOADSHEADERSTATE, "").toByteArray());
//    ui->splitter_2->restoreState(settings.value(SERIESPAGESPLITTERSTATE, "").toByteArray());

    connect(Controller::downloads(), &DownloadController::statusChanged,
            this, &MainWindow::enableActionsAccordingToDownloadStatus);

    enableActionsAccordingToDownloadSelection();
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
    settings.setValue(SERIESPAGESPLITTERSTATE, ui->splitter_2->saveState());

    delete ui;
}

void MainWindow::on_actionDownloads_triggered()
{
    ui->centralStackedWidget->setCurrentWidget(ui->pageDownloads);
}

void MainWindow::on_actionTV_Shows_triggered()
{
    ui->centralStackedWidget->setCurrentWidget(ui->pageSeries);
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

void MainWindow::enableActionsAccordingToDownloadStatus()
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

void MainWindow::enableActionsAccordingToDownloadSelection()
{
    bool sel = !ui->treeViewDownloads->selectionModel()->selectedRows().isEmpty()
            && ui->centralStackedWidget->currentWidget() == ui->pageDownloads;

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

void MainWindow::on_actionAdd_show_triggered()
{
    ui->actionTV_Shows->trigger();

    NewSeriesWizard wizard(this);
    wizard.exec();
}

void MainWindow::showSeasonsForSelectedSeries()
{
    m_episodesModel->setSeason(nullptr);

    QModelIndexList list = ui->treeViewSeries->selectionModel()->selectedRows();
    if(list.isEmpty()) {
        m_seasonsModel->setSeries(nullptr);
        return;
    }

    Series *series = m_seriesModel->seriesByIndex(list.first());
    if(!series) {
        m_seasonsModel->setSeries(nullptr);
        return;
    }

    m_seasonsModel->setSeries(series);
}

void MainWindow::showEpisodesForSelectedSeason()
{
    QModelIndexList list = ui->treeViewSeasons->selectionModel()->selectedRows();
    if(list.isEmpty()) {
        m_episodesModel->setSeason(nullptr);
        return;
    }

    Season *season = m_seasonsModel->seasonByIndex(list.first());
    if(!season) {
        m_episodesModel->setSeason(nullptr);
        return;
    }

    m_episodesModel->setSeason(season);
}
