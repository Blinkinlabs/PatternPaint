#ifndef EIGHTBYEIGHTCOMMANDS_H
#define EIGHTBYEIGHTCOMMANDS_H

#include "libblinkyglobal.h"

#include "serialcommand.h"

#include <QByteArray>

namespace EightByEightCommands {

enum FileMode {
    FileMode_Read = 0,
    FileMode_Write = 1,
};

enum Command {
    Command_Format_Filesystem = 0x10,
    Command_Open_File = 0x11,
    Command_Write = 0x12,
    Command_Read = 0x13,
    Command_Close_File = 0x14,
    Command_Lock_File_Access = 0x20,
    Command_Unlock_File_Access = 0x21,
    Command_GetFirmwareVersion = 0x30,
};

LIBBLINKY_EXPORT QByteArray commandHeader();

/// Format the SPIFFS partition
// TODO: Replace this with a command that just erases the animations?
LIBBLINKY_EXPORT SerialCommand formatFilesystem();

// Open a file
// Note: fileName less than ? characters
LIBBLINKY_EXPORT SerialCommand openFile(const QString fileName, FileMode mode);

// Write a block of data to the openend file (open in write mode first)
// Note: data length less than 256 bytes
LIBBLINKY_EXPORT SerialCommand writeChunk(const QByteArray &data);

// Write data to the opened file (can be arbitrary length)
LIBBLINKY_EXPORT QList<SerialCommand> write(const QByteArray &data);

// Verify a block of data from the openend file (open in read mode first)
// Note: data length less than 256 bytes
LIBBLINKY_EXPORT SerialCommand verifyChunk(const QByteArray &data);

// Verify a block of data from the openend file (open in read mode first)
LIBBLINKY_EXPORT QList<SerialCommand> verify(const QByteArray &data);

// Close the open file
LIBBLINKY_EXPORT SerialCommand closeFile();

// Lock file access - tell the EightByEight to stop accessing the filesystem
LIBBLINKY_EXPORT SerialCommand lockFileAccess();

// Unlock file access - tell the EightByEight to resume accessing the filesystem
LIBBLINKY_EXPORT SerialCommand unlockFileAccess();

// Get firmware version
LIBBLINKY_EXPORT SerialCommand getFirmwareVersion();

}

#endif // EIGHTBYEIGHTCOMMANDS_H
