/*
 * Copyright (C) 2008 Remko Troncon
 */

#include "SparkleAutoUpdater.h"

#include <Cocoa/Cocoa.h>
#include <Sparkle/Sparkle.h>

#include <QtCore/qdebug.h>

class SparkleAutoUpdater::Private
{
    public:
        SUUpdater* updater;
};

SparkleAutoUpdater::SparkleAutoUpdater(const QString &aUrl)
{
    d = new Private;

    d->updater = [SUUpdater sharedUpdater];
    [d->updater retain];

    NSURL* url = [NSURL URLWithString:
            [NSString stringWithUTF8String: aUrl.toUtf8().data()]];
    [d->updater setFeedURL: url];
}

SparkleAutoUpdater::~SparkleAutoUpdater()
{
    [d->updater release];
    delete d;
}


void SparkleAutoUpdater::setLanguage(const QString &language)
{
    Q_UNUSED(language)

    // TODO: Sparkle doesn't seem to allow for language overrides:
    // https://github.com/sparkle-project/Sparkle/issues/477
}

void SparkleAutoUpdater::init()
{
    // TODO: Do we actually have to call this?
    [d->updater checkForUpdatesInBackground];
}

void SparkleAutoUpdater::checkForUpdates()
{
    [d->updater checkForUpdates: 0];
}

bool SparkleAutoUpdater::getAutomatic()
{
 return [d->updater automaticallyChecksForUpdates];
}

void SparkleAutoUpdater::setAutomatic(bool setting)
{
    [d->updater setAutomaticallyChecksForUpdates: setting];
}
