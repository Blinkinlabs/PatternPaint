#ifndef AVR_109_COMMANDS_H
#define AVR_109_COMMANDS_H

#include "serialcommand.h"

#include <QList>

/// Subset of AVR109 commands supported by the Caterina bootloader
/// See Caterina.c in the Arduino sources, and the 'AVR109: Self Programming'
/// guide by Atmel for further information.

namespace Avr109Commands {
/// Write the contents of the flash
/// Note that if length is not a multiple of the page size, some pre-existing
/// data at the end may be erased.
/// @param data QByteArray containing the data to write to the flash
/// @param startAddress Page-aligned address to begin writing to, in bytes
QList<SerialCommand> writeFlash(QByteArray &data, int startAddress);

/// Verify the contents of the flash
/// @param data QByteArray containing the expected flash data
/// @param startAddress Page-aligned address to verify from, in bytes
QList<SerialCommand> verifyFlash(QByteArray &data, int startAddress);

/// Write a page of flash data
/// Note that the address must be set by running setAddress() before this
/// command.
/// @param page Page data to write
SerialCommand writeFlashBlock(QByteArray data);

QList<SerialCommand> writeEeprom(QByteArray &data, int startAddress);

SerialCommand writeEepromBlock(QByteArray data);

/// Set the current read/write address for flash/EEPROM reading/writing
/// Note: The address needs to be divisble by 2, because it is interpreted
/// as a word address by the bootloader.
/// @param address Offset in the device flash space to use for the next read
/// or write operation
SerialCommand setAddressWord(int address);

/// Check that we are talking to the correct device
SerialCommand checkDeviceSignature();

/// Instruct the programmer to reset.
SerialCommand reset();
}

#endif // AVR_109_COMMANDS_H
