/*
 * Copyright (C) 2008 Remko Troncon
 */

#ifndef AUTOUPDATER_H
#define AUTOUPDATER_H

#include <QString>

class AutoUpdater
{
public:
    virtual ~AutoUpdater();

    /// Set the language that should be used to display update messages
    virtual void setLanguage(const QString &language) = 0;

    /// Initialize the updater, and start checking for updates if auto-updates are enabled.
    virtual void init() = 0;

    /// Force an update check, regardless of rate limiting
    virtual void checkForUpdates() = 0;

    /// Determine if the auto-updater will run at startup
    /// @return true if the auto-updater will run at startup, false otherwise
    virtual bool getAutomatic() = 0;

    /// Determine if the auto-updater will run at startup
    /// @param true if the auto-updater will run at startup, false otherwise
    virtual void setAutomatic(bool setting) = 0;
};

#endif
