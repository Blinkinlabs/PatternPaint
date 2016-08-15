#include <QApplication>
#include <QSettings>
#include <QString>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
//    Q_INIT_RESOURCE(images);  // TODO: Do we need to do this?
//    Q_INIT_RESOURCE(examples);

    QApplication app(argc, argv);
    app.setOrganizationName(ORGANIZATION_NAME);
    app.setOrganizationDomain(ORGANIZATION_DOMAIN);
    app.setApplicationName(APPLICATION_NAME);
    //app.setApplicationVersion(???); // TODO??

    qSetMessagePattern("%{type} %{function}: %{message}");

    MainWindow mainWindow;
    mainWindow.show();

    mainWindow.windowLoaded();  // TODO: Why?


    return app.exec();
}
