#include "winsparkleautoupdater.h"
#include <winsparkle.h>

class WinSparkleAutoUpdater::Private
{
public:
    QString url;
};

WinSparkleAutoUpdater::WinSparkleAutoUpdater(const QString &url)
{
    d = new Private();
    d->url = url;

    win_sparkle_set_appcast_url(d->url.toUtf8().constData());

#define W(x)          W_(x)
#define W_(x)         L ## x

    win_sparkle_set_app_details(
        W(ORGANIZATION_NAME),
        W(APPLICATION_NAME),
        W(GIT_VERSION));
}

void WinSparkleAutoUpdater::setLanguage(const QString &language)
{
    win_sparkle_set_lang(language.toUtf8().constData());
}

void WinSparkleAutoUpdater::init()
{
    // Initialize the updater and start auto-updating
    win_sparkle_init();
}

void WinSparkleAutoUpdater::checkForUpdates()
{
    win_sparkle_check_update_with_ui();
}

// TODO: Test in Windows
bool WinSparkleAutoUpdater::getAutomatic()
{
    return win_sparkle_get_automatic_check_for_updates() == 1;
}

// TODO: Test in Windows
void WinSparkleAutoUpdater::setAutomatic(bool setting)
{
    win_sparkle_set_automatic_check_for_updates(setting ? 1 : 0);
}

WinSparkleAutoUpdater::~WinSparkleAutoUpdater()
{
    win_sparkle_cleanup();
    delete d;
}
