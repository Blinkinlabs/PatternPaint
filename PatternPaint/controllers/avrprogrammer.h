#ifndef AVRPROGRAMMER_H
#define AVRPROGRAMMER_H

#include "serialcommandqueue.h"

/// Given a serial object that is suspected to have an AVR109-compatible booloader
/// attached to it (ie, caterina), use it to load a new user program.
class AvrProgrammer : public SerialCommandQueue
{
    Q_OBJECT
public:
    explicit AvrProgrammer(QObject *parent = 0);

    /// Read the contents of the flash
    /// @param startAddress Word-aligned address to begin reading from, in bytes
    /// @param lengthBytes Length of data to read, in bytes
    void readFlash(int startAddress, int lengthBytes);

    /// Write the contents of the flash
    /// Note that if length is not a multiple of the page size, some pre-existing
    /// data at the end may be erased.
    /// @param data QByteArray containing the data to write to the flash
    /// @param startAddress Page-aligned address to begin writing to, in bytes
    void writeFlash(QByteArray &data, int startAddress);

    /// These functions are direct implementations of the bootloader interface

    /// Set the current read/write address for flash/EEPROM reading/writing
    /// Note: The address needs to be divisble by 2, because it is interpreted
    /// as a word address by the bootloader.
    /// @param address Offset in the device flash space to use for the next read
    /// or write operation
    void setAddress(int address);

    /// Check that we are talking to the correct device
    void checkDeviceSignature();

    /// Instruct the programmer to reset.
    void reset();
};

#endif // AVRPROGRAMMER_H
