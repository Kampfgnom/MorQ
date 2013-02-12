#include "preferenceswindow.h"
#include "ui_preferenceswindow.h"

#include "preferences.h"

PreferencesWindow::PreferencesWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PreferencesWindow)
{
    ui->setupUi(this);

    ui->lineEditUserName->setText(Preferences::premiumizeMeUserName());
    ui->lineEditPassword->setText(Preferences::premiumizeMeUserPassword());

    QActionGroup *viewActionGroup = new QActionGroup(this);
    viewActionGroup->addAction(ui->actionPremuimizeMe);

    setAttribute(Qt::WA_DeleteOnClose, true);
}

PreferencesWindow::~PreferencesWindow()
{
    delete ui;
}

void PreferencesWindow::on_lineEditUserName_editingFinished()
{
    Preferences::setPremiumizeMeUserName(ui->lineEditUserName->text());
    Preferences::setPremiumizeMeUserPassword(ui->lineEditPassword->text());
}
