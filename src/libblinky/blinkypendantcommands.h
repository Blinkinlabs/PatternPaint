#ifndef BLINKYPENDANTCOMMANDS_H
#define BLINKYPENDANTCOMMANDS_H

#include "libblinkyglobal.h"

#include "serialcommand.h"

#include <QByteArray>
#include <QList>

namespace BlinkyPendantCommands {

LIBBLINKY_EXPORT QByteArray commandHeader();

LIBBLINKY_EXPORT SerialCommand startWrite();

LIBBLINKY_EXPORT SerialCommand writeFlashChunk(const QByteArray &data);

LIBBLINKY_EXPORT QList<SerialCommand> writeFlash(const QByteArray &data);

LIBBLINKY_EXPORT SerialCommand stopWrite();

}

#endif // BLINKYPENDANTCOMMANDS_H
