#ifndef AVR_PROGRAMMER_COMMANDS
#define AVR_PROGRAMMER_COMMANDS

#include "serialcommand.h"

#include <QList>

namespace Avr109Commands {
///// Read the contents of the flash
///// @param startAddress Word-aligned address to begin reading from, in bytes
///// @param lengthBytes Length of data to read, in bytes
// void readFlash(int startAddress, int lengthBytes);

/// Write the contents of the flash
/// Note that if length is not a multiple of the page size, some pre-existing
/// data at the end may be erased.
/// @param data QByteArray containing the data to write to the flash
/// @param startAddress Page-aligned address to begin writing to, in bytes
QList<SerialCommand> writeFlash(QByteArray &data, int startAddress);

/// Write a page of flash data
/// Note that the address must be set by running setAddress() before this
/// command.
/// @param page Page data to write
SerialCommand writeFlashPage(QByteArray page);

/// Set the current read/write address for flash/EEPROM reading/writing
/// Note: The address needs to be divisble by 2, because it is interpreted
/// as a word address by the bootloader.
/// @param address Offset in the device flash space to use for the next read
/// or write operation
SerialCommand setAddress(int address);

/// Check that we are talking to the correct device
SerialCommand checkDeviceSignature();

/// Instruct the programmer to reset.
SerialCommand reset();
}

#endif // AVR_PROGRAMMER_COMMANDS
