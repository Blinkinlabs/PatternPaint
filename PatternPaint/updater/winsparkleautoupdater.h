#ifndef WINSPARKLEAUTOUPDATER_H
#define WINSPARKLEAUTOUPDATER_H

#include <QString>

#include "AutoUpdater.h"

class WinSparkleAutoUpdater : public AutoUpdater
{
public:
    WinSparkleAutoUpdater(const QString &url);
    ~WinSparkleAutoUpdater();

    void checkForUpdates();

    bool getAutomatic();
    void setAutomatic(bool setting);
private:
    class Private;
    Private *d;
};

#endif // WINSPARKLEAUTOUPDATER_H
