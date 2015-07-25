#include "mainwindow.h"
#include <QApplication>
#include "autoupdater.h"

#ifdef Q_OS_MAC
#include "CocoaInitializer.h"
#include "SparkleAutoUpdater.h"
#endif

#define OSX_RELEASE_APPCAST_DEFAULT "http://software.blinkinlabs.com/patternpaint/patternpaint-osx.xml"
#define WINDOWNS_RELEASE_APPCAST_DEFAULT "http://software.blinkinlabs.com/patternpaint/patternpaint-windows.xml"

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

  QSettings settings;
  QString updateUrl = settings.value("Udpates/osx-release-appcast", OSX_RELEASE_APPCAST_DEFAULT).toString();

  updater = new SparkleAutoUpdater(updateUrl);
#endif

    if (updater) {
      updater->checkForUpdates();
    }

    MainWindow w;
    w.show();

    return a.exec();
}
