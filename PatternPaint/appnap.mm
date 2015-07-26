#include "appnap.h"

#include <Foundation/NSProcessInfo.h>
#include <Foundation/Foundation.h>

class CAppNapInhibitor::Private
{
public:
    id<NSObject> activityId;
};

CAppNapInhibitor::CAppNapInhibitor(const char* strReason) : d(new Private)
{
#if defined(MAC_OS_X_VERSION_MAX_ALLOWED) &&  MAC_OS_X_VERSION_MAX_ALLOWED >= 1090
    if ([[NSProcessInfo processInfo] respondsToSelector:@selector(beginActivityWithOptions:reason:)])
        d->activityId = [[NSProcessInfo processInfo ] beginActivityWithOptions: NSActivityUserInitiatedAllowingIdleSystemSleep reason:@(strReason)];
#endif
}

CAppNapInhibitor::~CAppNapInhibitor()
{
#if defined(MAC_OS_X_VERSION_MAX_ALLOWED) &&  MAC_OS_X_VERSION_MAX_ALLOWED >= 1090
    if ([[NSProcessInfo processInfo] respondsToSelector:@selector(endActivity:)])
        [[NSProcessInfo processInfo] endActivity:d->activityId];
#endif
    delete d;
}
