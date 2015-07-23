#include "mainwindow.h"
#include <QApplication>
#include "autoupdater.h"

#ifdef Q_OS_MAC
#include "CocoaInitializer.h"
#include "SparkleAutoUpdater.h"
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName(ORGANIZATION_NAME);
    a.setOrganizationDomain(ORGANIZATION_DOMAIN);
    a.setApplicationName(APPLICATION_NAME);

    qSetMessagePattern("%{type} %{function}: %{message}");

    AutoUpdater* updater = 0;

#ifdef Q_OS_MAC
  CocoaInitializer cocoaInitiarizer;
  updater = new SparkleAutoUpdater("blinkinlabs.com/updates/patternpaint/patternpaint-osx.xml");
#endif

    if (updater) {
      updater->checkForUpdates();
    }

    MainWindow w;
    w.show();

    return a.exec();
}
