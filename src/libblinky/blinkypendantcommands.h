#ifndef BLINKYPENDANTCOMMANDS_H
#define BLINKYPENDANTCOMMANDS_H

#include "serialcommand.h"

#include <QByteArray>
#include <QList>

namespace BlinkyPendantCommands {

QByteArray commandHeader();

SerialCommand startWrite();

SerialCommand writeFlashChunk(const QByteArray &data);

QList<SerialCommand> writeFlash(const QByteArray &data);

SerialCommand stopWrite();

}

#endif // BLINKYPENDANTCOMMANDS_H
