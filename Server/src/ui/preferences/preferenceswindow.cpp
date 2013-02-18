#include "preferenceswindow.h"
#include "ui_preferenceswindow.h"

#include "preferences.h"

#include <QFileDialog>

PreferencesWindow::PreferencesWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PreferencesWindow)
{
    ui->setupUi(this);

    ui->lineEditUserName->setText(Preferences::premiumizeMeUserName());
    ui->lineEditPassword->setText(Preferences::premiumizeMeUserPassword());

    ui->checkBoxEnableExtract->setChecked(Preferences::extractDownloads());
    ui->lineEditExtractFolder->setEnabled(Preferences::extractDownloads());
    ui->pushButtonChooseExtractFolder->setEnabled(Preferences::extractDownloads());

    ui->lineEditDownloadFolder->setText(Preferences::downloadFolder());
    ui->lineEditExtractFolder->setText(Preferences::extractFolder());

    ui->spinBoxMaxDownloads->setValue(Preferences::maxDownloads());

    QActionGroup *viewActionGroup = new QActionGroup(this);
    viewActionGroup->addAction(ui->actionDownloads);
    viewActionGroup->addAction(ui->actionPremuimizeMe);

    setAttribute(Qt::WA_DeleteOnClose, true);
}

PreferencesWindow::~PreferencesWindow()
{
    delete ui;
}

void PreferencesWindow::setCurrentPage(PreferencesWindow::Page page)
{
    switch(page) {
    default:
    case DownloadsPage:
        on_actionDownloads_triggered();
        break;
    case PremiumizeMePage:
        on_actionPremuimizeMe_triggered();
        break;
    }
}

void PreferencesWindow::on_lineEditUserName_editingFinished()
{
    Preferences::setPremiumizeMeUserName(ui->lineEditUserName->text());
}

void PreferencesWindow::on_lineEditPassword_editingFinished()
{
    Preferences::setPremiumizeMeUserPassword(ui->lineEditPassword->text());
}

void PreferencesWindow::on_checkBoxEnableExtract_clicked()
{
    ui->lineEditExtractFolder->setEnabled(ui->checkBoxEnableExtract->isChecked());
    ui->pushButtonChooseExtractFolder->setEnabled(ui->checkBoxEnableExtract->isChecked());
    Preferences::setExtractDownloads(ui->checkBoxEnableExtract->isChecked());
}

void PreferencesWindow::on_actionPremuimizeMe_triggered()
{
    ui->actionPremuimizeMe->setChecked(true);
    ui->stackedWidget->setCurrentWidget(ui->pagePremiumize);
}

void PreferencesWindow::on_actionDownloads_triggered()
{
    ui->actionDownloads->setChecked(true);
    ui->stackedWidget->setCurrentWidget(ui->pageDownloads);
}

void PreferencesWindow::on_lineEditDownloadFolder_editingFinished()
{
    Preferences::setDownloadFolder(ui->lineEditDownloadFolder->text());
}

void PreferencesWindow::on_lineEditExtractFolder_editingFinished()
{
    Preferences::setExtractFolder(ui->lineEditExtractFolder->text());
}

void PreferencesWindow::on_pushButtonChooseDownloadFolder_clicked()
{
    QString folder = QFileDialog::getExistingDirectory(this);
    if(folder.isEmpty())
        return;

    ui->lineEditDownloadFolder->setText(folder);
    Preferences::setDownloadFolder(folder);
}

void PreferencesWindow::on_pushButtonChooseExtractFolder_clicked()
{
    QString folder = QFileDialog::getExistingDirectory(this);
    if(folder.isEmpty())
        return;

    ui->lineEditExtractFolder->setText(folder);
    Preferences::setExtractFolder(folder);
}

void PreferencesWindow::on_spinBoxMaxDownloads_editingFinished()
{
    Preferences::setMaxDownloads(ui->spinBoxMaxDownloads->value());
}
