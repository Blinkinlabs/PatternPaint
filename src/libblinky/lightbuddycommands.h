#ifndef LIGHTBUDDYCOMMANDS_H
#define LIGHTBUDDYCOMMANDS_H

#include "libblinkyglobal.h"

#include "serialcommand.h"

#include <QByteArray>

namespace LightBuddyCommands {

LIBBLINKY_EXPORT QByteArray commandHeader();

/// Erase the
LIBBLINKY_EXPORT SerialCommand eraseFlash();

/// Create a new file, with the given size. The response
/// @param Size of the file, in bytes. Must be smaller than the maximum
/// file size for the controller, or the operation will fail
LIBBLINKY_EXPORT SerialCommand fileNew(uint32_t sizeBytes);

LIBBLINKY_EXPORT SerialCommand writePage(int sector, int offset, const QByteArray &data);

LIBBLINKY_EXPORT SerialCommand reloadAnimations();
}

#endif // LIGHTBUDDYCOMMANDS_H
