#include <QApplication>

#include "controller/controller.h"
#include "ui/mainwindow/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setApplicationName("MorQServer");
    QApplication::setOrganizationName("LBProductions");
    QApplication::setApplicationVersion("VERSION");

    Controller::initialize();
    MainWindow::instance()->show();

    return a.exec();
}
