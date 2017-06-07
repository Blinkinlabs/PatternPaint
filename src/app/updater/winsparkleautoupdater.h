#ifndef WINSPARKLEAUTOUPDATER_H
#define WINSPARKLEAUTOUPDATER_H

#include "AutoUpdater.h"

class WinSparkleAutoUpdater : public AutoUpdater
{
public:
    WinSparkleAutoUpdater(const QString &url);
    ~WinSparkleAutoUpdater();

    void setLanguage(const QString &language);

    void init();

    void checkForUpdates();

    bool getAutomatic();
    void setAutomatic(bool setting);
private:
    class Private;
    Private *d;
};

#endif // WINSPARKLEAUTOUPDATER_H
