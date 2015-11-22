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

SparkleAutoUpdater::SparkleAutoUpdater(const QString& aUrl)
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

void SparkleAutoUpdater::checkForUpdates()
{
    [d->updater checkForUpdatesInBackground];
}

bool SparkleAutoUpdater::getAutomatic()
{
 return [d->updater automaticallyChecksForUpdates];
}

void SparkleAutoUpdater::setAutomatic(bool setting)
{
    qDebug() << "     *** [Lib] {MegaUp}: set automatic update checking";
    [d->updater setAutomaticallyChecksForUpdates: setting];
    qDebug() << "     ***                 done";
}
