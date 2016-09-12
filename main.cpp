#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("SaveGameBackup");
    a.setApplicationVersion("0.0.1");
    a.setOrganizationName("h4mu");
    MainWindow w;
    w.show();

    return a.exec();
}
