#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName(ORGANIZATION_NAME);
    a.setOrganizationDomain(ORGANIZATION_DOMAIN);
    a.setApplicationName(APPLICATION_NAME);

    MainWindow w;
    w.show();

    return a.exec();
}
