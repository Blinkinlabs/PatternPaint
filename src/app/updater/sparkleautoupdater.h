/*
 * Copyright (C) 2008 Remko Troncon
 */

#ifndef SPARKLEAUTOUPDATER_H
#define SPARKLEAUTOUPDATER_H

#include "AutoUpdater.h"

class SparkleAutoUpdater : public AutoUpdater
{
public:
    SparkleAutoUpdater(const QString &url);
    ~SparkleAutoUpdater();

    void setLanguage(const QString &language);

    void init();

    void checkForUpdates();

    bool getAutomatic();
    void setAutomatic(bool setting);

private:
    class Private;
    Private *d;
};

#endif
