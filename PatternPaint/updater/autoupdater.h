/*
 * Copyright (C) 2008 Remko Troncon
 */

#ifndef AUTOUPDATER_H
#define AUTOUPDATER_H

class AutoUpdater
{
public:
    virtual ~AutoUpdater();

    /// Check for updates now
    virtual void checkForUpdates() = 0;

    /// Determine if the auto-updater will run at startup
    /// @return true if the auto-updater will run at startup, false otherwise
    virtual bool getAutomatic() = 0;

    /// Determine if the auto-updater will run at startup
    /// @return true if the auto-updater will run at startup, false otherwise
    virtual void setAutomatic(bool) = 0;
};

#endif
