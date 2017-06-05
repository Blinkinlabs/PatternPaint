#ifndef ESPBOOTLOADERCOMMANDS_H
#define ESPBOOTLOADERCOMMANDS_H

#include "libblinkyglobal.h"

#include <QByteArray>
#include <QObject>

#include "serialcommand.h"

// Based on this guide:
// https://github.com/espressif/esptool/wiki/Serial-Protocol

// With bitfields here:
// https://gist.github.com/bvernoux/6ddc02ed157417a4df37861488357c5a

namespace Esp8266BootloaderCommands {
//    Q_NAMESPACE

enum Opcode {
    Opcode_FlashDownloadStart = 0x02,
    Opcode_FlashDownloadData = 0x03,
    Opcode_FlashDownloadFinish = 0x04,
    Opcode_RamDownloadStart = 0x05,
    Opcode_RamDownloadFinish = 0x06,
    Opcode_RamDownloadData = 0x07,
    Opcode_SyncFrame = 0x08,
    Opcode_WriteRegister = 0x09,
    Opcode_ReadRegister = 0x0a,
    Opcode_ConfigureSPIParameters = 0x0b
};

// TODO: This doesn't seem to work on Windows.
//Q_ENUM_NS(Opcode)

LIBBLINKY_EXPORT unsigned char calculateChecksum(const QByteArray &data);

LIBBLINKY_EXPORT QByteArray buildCommand(Opcode opcode, const QByteArray &data, unsigned char checksum);

LIBBLINKY_EXPORT QByteArray slipEncode(const QByteArray &data);
LIBBLINKY_EXPORT QByteArray slipDecode(const QByteArray &data);

LIBBLINKY_EXPORT SerialCommand flashDownloadStart(unsigned int totalSize,
                                 unsigned int numberOfBlocks,
                                 unsigned int blockSize,
                                 unsigned int offset);

LIBBLINKY_EXPORT SerialCommand flashDownloadData(unsigned int sequence, QByteArray flashData);

LIBBLINKY_EXPORT SerialCommand flashDownloadFinish(unsigned int rebootFlag);

LIBBLINKY_EXPORT SerialCommand SyncFrame();

}
Q_DECLARE_METATYPE(Esp8266BootloaderCommands::Opcode)

#endif // ESPBOOTLOADERCOMMANDS_H
