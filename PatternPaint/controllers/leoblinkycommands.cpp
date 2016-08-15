#include "leoblinkycommands.h"

#include <QDebug>

//#define COMMAND_DISPLAY_FRAME 0x00
#define COMMAND_ERASE_FLASH   0x01
#define COMMAND_WRITE_PAGE    0x02
#define COMMAND_RELOAD_ANIMATIONS    0x03

#define PAGE_SIZE_BYTES     256    // Size unit for writes to flash

namespace LeoBlinkyCommands {

QByteArray encodeInt(int data)
{
    QByteArray output;
    output.append(char((data >> 24) & 0xFF));
    output.append(char((data >> 16) & 0xFF));
    output.append(char((data >>  8) & 0xFF));
    output.append(char((data) & 0xFF));
    return output;
}

QByteArray encodeUint16(int data)
{
    QByteArray output;
    output.append(char((data >>  8) & 0xFF));
    output.append(char((data) & 0xFF));
    return output;
}

QByteArray encapsulatePacket(QByteArray packet)
{
    QByteArray output;
    output += encodeUint16(0xdead);  // Header (2 bytes, MSB first)
    output += encodeUint16(packet.length());  // Payload length (2 bytes, MSB first) TODO
    output += packet;
    output.append(char(0x00));  // CRC

//    qDebug() << " packet length:" << packet.length()
//            << " encapsulated length:" << output.length()
//            << " " << uint8_t(output[0])
//            << " " << uint8_t(output[1])
//            << " " << uint8_t(output[2])
//            << " " << uint8_t(output[3])
//            << " " << uint8_t(output[4]);

    return output;
}

SerialCommand eraseFlash()
{
    QByteArray command;

    command.append(char(COMMAND_ERASE_FLASH)); // Erase flash

    QByteArray ret;
    ret.append('!');

    QByteArray mask;
    mask.append(char(0xFF));

    // Note: only the length matters for the response, the response data
    // will be ignored.
    return SerialCommand("eraseFlash", encapsulatePacket(command), ret, mask, 20000);
}

SerialCommand writePage(int address, QByteArray data)
{
    if (data.size() != PAGE_SIZE_BYTES) {
        // TODO: How to error out here?
        qCritical() << "Data size not multiple of page size!";
    }

    QByteArray command;

    command.append(char(COMMAND_WRITE_PAGE));     // Write page
    command += encodeInt(address);
    command += data;

    QByteArray ret;
    ret.append('!');

    QByteArray mask;
    mask.append(char(0xFF));

//    qDebug() << "writePage address=" << address << " length:" << command.length()
//            << " " << int(command[0])
//            << " " << int(command[1])
//            << " " << int(command[2])
//            << " " << int(command[3])
//            << " " << int(command[4]);

    return SerialCommand("writePage", encapsulatePacket(command), ret, mask, 2000);
}

SerialCommand reloadAnimations()
{
    QByteArray command;

    command.append(char(COMMAND_RELOAD_ANIMATIONS)); // Reload the animations table

    QByteArray ret;
    ret.append('!');

    QByteArray mask;
    mask.append(char(0xFF));

    // Note: only the length matters for the response, the response data
    // will be ignored.
    return SerialCommand("reloadAnimations", encapsulatePacket(command), ret, mask, 2000);
}

QList<SerialCommand> writeData(QByteArray &data)
{
    QList<SerialCommand> commands;

    if (data.length() == 0) {
        // TODO: How to report an error here?
        qCritical() << "No data to write";
        return commands;
    }

//    // Pad the data to the page size
//    while (data.length()%PAGE_SIZE_BYTES != 0)
//        data.append(char(0xFF));

    for (int currentChunkPosition = 0;
        currentChunkPosition < data.length();
        currentChunkPosition += PAGE_SIZE_BYTES) {

        commands.append(writePage(currentChunkPosition,
                                  data.mid(currentChunkPosition, PAGE_SIZE_BYTES)));
    }

    return commands;
}

}
