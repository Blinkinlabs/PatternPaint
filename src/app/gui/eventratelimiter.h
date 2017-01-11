#ifndef EVENTRATELIMITER_H
#define EVENTRATELIMITER_H

#include <QtGlobal>

/// Filter
class intervalFilter
{
public:
    /// Start a new interval filter.
    /// @param minimumInterval Minimum elapsed time between events, in ms
    intervalFilter(qint64 minimumInterval);

    /// Check if enough time has elapsed since the last event
    bool check();

    /// Set the last event time to now
    void force();

private:
    qint64 minimumInterval;
    qint64 lastEventTime;
};

#endif // EVENTRATELIMITER_H
