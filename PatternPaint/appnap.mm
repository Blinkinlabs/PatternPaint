#include "appnap.h"

#include <Foundation/NSProcessInfo.h>
#include <Foundation/Foundation.h>

class CAppNapInhibitor::Private
{
public:
    id<NSObject> activityId;
};

CAppNapInhibitor::CAppNapInhibitor(const char* strReason) :
    d(new Private)
{
#if defined(MAC_OS_X_VERSION_MAX_ALLOWED) &&  MAC_OS_X_VERSION_MAX_ALLOWED >= 1090
    auto processInfo = [NSProcessInfo processInfo];
    if ([processInfo respondsToSelector:@selector(beginActivityWithOptions:reason:)])
        d->activityId = [[processInfo beginActivityWithOptions:NSActivityUserInitiatedAllowingIdleSystemSleep
                                                 reason:[NSString stringWithUTF8String:strReason]]
                  retain];

#endif
}

CAppNapInhibitor::~CAppNapInhibitor()
{
#if defined(MAC_OS_X_VERSION_MAX_ALLOWED) &&  MAC_OS_X_VERSION_MAX_ALLOWED >= 1090
    auto processInfo = [NSProcessInfo processInfo];
    if (d->activityId && [processInfo respondsToSelector:@selector(endActivity:)])
        [processInfo endActivity:(id)d->activityId];
    [(id)d->activityId release];

#endif
    delete d;
}
