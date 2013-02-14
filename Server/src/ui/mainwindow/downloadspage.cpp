#include "downloadspage.h"
#include "ui_downloadspage.h"

#include "downloadsitemdelegate.h"

#include "controller/downloadcontroller.h"

DownloadsPage::DownloadsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadsPage)
{
    ui->setupUi(this);

    ui->downloadsTreeView->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->downloadsTreeView->setModel(Controller::downloads()->model());
    ui->downloadsTreeView->setItemDelegateForColumn(DownloadsItemModel::UserInputColumn, new DownloadsItemDelegate(this));
}

DownloadsPage::~DownloadsPage()
{
    delete ui;
}
