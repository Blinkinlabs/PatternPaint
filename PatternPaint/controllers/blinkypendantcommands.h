#ifndef BLINKYPENDANTCOMMANDS_H
#define BLINKYPENDANTCOMMANDS_H

#include "serialcommand.h"

#include <QByteArray>
#include <QList>

namespace BlinkyPendantCommands {

SerialCommand startWrite();

SerialCommand writeFlashPage(QByteArray data);

QList<SerialCommand> writeData(QByteArray &data);

SerialCommand stopWrite();

}

#endif // BLINKYPENDANTCOMMANDS_H
