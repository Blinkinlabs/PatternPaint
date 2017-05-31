#ifndef ESPBOOTLOADERCOMMANDS_H
#define ESPBOOTLOADERCOMMANDS_H

#include <QByteArray>

#include "serialcommand.h"

// Based on this guide:
// https://github.com/espressif/esptool/wiki/Serial-Protocol

// With bitfields here:
// https://gist.github.com/bvernoux/6ddc02ed157417a4df37861488357c5a

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
    Opcode_ReadRegister = 0x0a,
    Opcode_ConfigureSPIParameters = 0x0b,
};

char calculateChecksum(const QByteArray &data);

QByteArray slipEncode(const QByteArray &data);
QByteArray slipDecode(const QByteArray &data);

SerialCommand SyncFrame();

SerialCommand readRegister(unsigned int address);

}

#endif // ESPBOOTLOADERCOMMANDS_H
