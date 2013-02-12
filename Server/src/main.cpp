#include <QApplication>

#include "controller/controller.h"
#include "ui/mainwindow/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Controller::initialize();
    MainWindow::instance()->show();

    return a.exec();
}
