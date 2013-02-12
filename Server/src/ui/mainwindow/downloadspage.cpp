#include "downloadspage.h"
#include "ui_downloadspage.h"

#include "controller/downloadcontroller.h"

DownloadsPage::DownloadsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadsPage)
{
    ui->setupUi(this);

    ui->downloadsTreeView->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->downloadsTreeView->setModel(Controller::downloads()->model());
}

DownloadsPage::~DownloadsPage()
{
    delete ui;
}
