#include "mainwindow.h"
#include <QApplication>
#include "autoupdater.h"

#if defined Q_OS_MAC
#include "CocoaInitializer.h"
#include "SparkleAutoUpdater.h"
#elif defined Q_OS_WIN
#include "winsparkleautoupdater.h"
#endif

#if defined Q_OS_MAC
#include "appnap.h"
#endif

#define OSX_RELEASE_APPCAST_DEFAULT "http://software.blinkinlabs.com/patternpaint/patternpaint-osx.xml"
#define WINDOWS_RELEASE_APPCAST_DEFAULT "http://software.blinkinlabs.com/patternpaint/patternpaint-windows.xml"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName(ORGANIZATION_NAME);
    a.setOrganizationDomain(ORGANIZATION_DOMAIN);
    a.setApplicationName(APPLICATION_NAME);

    qSetMessagePattern("%{type} %{function}: %{message}");

    AutoUpdater* updater = 0;

#if defined(Q_OS_MAC)
    CocoaInitializer cocoaInitiarizer;

    QSettings settings;
    QString updateUrl = settings.value("Updates/releaseAppcastUrl", OSX_RELEASE_APPCAST_DEFAULT).toString();

#if defined(DISABLE_UPDATE_CHECKS)
#warning Updater initialization disabled
#else
    updater = new SparkleAutoUpdater(updateUrl);
#endif

#elif defined(Q_OS_WIN)
    QSettings settings;
    QString updateUrl = settings.value("Updates/releaseAppcastUrl", WINDOWS_RELEASE_APPCAST_DEFAULT).toString();

#if defined(DISABLE_UPDATE_CHECKS)
#warning Updater initialization disabled
#else
    updater = new WinSparkleAutoUpdater(updateUrl);
#endif

#endif

    if (updater) {
        // TODO: Defer this until after the app opens on Windows (to prevent the dialog from being hidden?)
        updater->checkForUpdates();
    }

#if defined Q_OS_MAC
    CAppNapInhibitor appNapInhibitor("Interaction with hardware");
#endif

    MainWindow w;
    w.show();

    return a.exec();
}
