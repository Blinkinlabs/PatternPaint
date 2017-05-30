#include "esp8266bootloadercommands.h"
#include "bytearrayhelpers.h"

#include <QDebug>

namespace Esp8266BootloaderCommands {

//SerialCommand flashDownloadStart(unsigned int size, unsigned int blockCount, unsigned int blockSize, unsigned int offset)
//{
//    QByteArray command;
//    command.append(0x00); // direction
//    command.append((char)Opcode_FlashDownloadStart);
//    command.append(uint16ToByteArrayLittle(4*4)); // size, blockCount, blockSize, size
//    //command.append(checksum); // TODO?
//    command.append(uint32ToByteArrayLittle(size));
//    command.append(uint32ToByteArrayLittle(blockCount));
//    command.append(uint32ToByteArrayLittle(blockSize));
//    command.append(uint32ToByteArrayLittle(offset));

//    return SerialCommand();
//}



}
