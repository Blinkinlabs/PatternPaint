#ifndef LEOBLINKYCOMMANDS_H
#define LEOBLINKYCOMMANDS_H

#include "serialcommand.h"

#include <QByteArray>

namespace LeoBlinkyCommands {
/// Erase the flash
SerialCommand eraseFlash();

/// Writes a 256 byte page at the given address
SerialCommand writePage(int address, QByteArray data);

/// Reload the animations table
SerialCommand reloadAnimations();

/// Write a large block of data starting at 0, padded to page length
QList<SerialCommand> writeData(QByteArray &data);
}

#endif // LEOBLINKYCOMMANDS_H
