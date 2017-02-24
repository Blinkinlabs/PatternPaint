#include "intervalfilter.h"

#include <QDateTime>

IntervalFilter::IntervalFilter(qint64 minimumInterval) :
    minimumInterval(minimumInterval),
    lastEventTime(0)
{
}

bool IntervalFilter::check()
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

    bool elapsed = (currentTime - lastEventTime) > minimumInterval;

    if(elapsed)
        lastEventTime = currentTime;

    return elapsed;
}

void IntervalFilter::force()
{
    lastEventTime = QDateTime::currentMSecsSinceEpoch();
}
