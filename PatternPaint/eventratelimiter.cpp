#include "eventratelimiter.h"

#include <QDateTime>

intervalFilter::intervalFilter(qint64 minimumInterval) :
    minimumInterval(minimumInterval),
    lastEventTime(0)
{
}

bool intervalFilter::check()
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

    bool elapsed = (currentTime - lastEventTime) > minimumInterval;

    if(elapsed)
        lastEventTime = currentTime;

    return elapsed;
}

void intervalFilter::force()
{
    lastEventTime = QDateTime::currentMSecsSinceEpoch();
}
