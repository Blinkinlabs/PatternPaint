#ifndef ESPBOOTLOADERCOMMANDS_H
#define ESPBOOTLOADERCOMMANDS_H

#include <QByteArray>

#include "serialcommand.h"

// Based on this guide:
// https://github.com/espressif/esptool/wiki/Serial-Protocol

namespace Esp8266BootloaderCommands {

enum Opcode {
    Opcode_FlashDownloadStart = 0x02,
    Opcode_FlahsDownloadData = 0x03,
    Opcode_FlashDownloadFinish = 0x04,
    Opcode_RamDownloadStart = 0x05,
    Opcode_RamDownloadFinish = 0x06,
    Opcode_RamDownloadData = 0x07,
    Opcode_SyncFrame = 0x08,
    Opcode_WriteRegister = 0x09,
    Opcode_ReadRegister = 0x01,
    Opcode_ConfigureSPIParameters = 0x0b,
};

//QByteArray calculateChecksum(const QByteArray &data);

//QByteArray slipEncode(const QByteArray &data);
//QByteArray slipDeccode(const QByteArray &data);

//SerialCommand flashDownloadStart(unsigned int size,
//                                 unsigned int blockCount,
//                                 unsigned int blockSize,
//                                 unsigned int offset);

}

#endif // ESPBOOTLOADERCOMMANDS_H
