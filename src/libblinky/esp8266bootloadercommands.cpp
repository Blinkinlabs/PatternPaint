#include "esp8266bootloadercommands.h"
#include "bytearrayhelpers.h"

#include <QDebug>

namespace Esp8266BootloaderCommands {

char calculateChecksum(const QByteArray &data)
{
    // From https://github.com/espressif/esptool/wiki/Serial-Protocol:
    // Each byte in the payload is XOR'ed together, as well as the magic number
    // 0xEF. The result is stored as a zero-padded byte in the 32-bit checksum
    // field in the header.

    char checksum = (char)0xEF;

    for(char byte : data) {
        checksum ^= byte;
    }

    return checksum;
}

QByteArray slipEncode(const QByteArray &data)
{
    // From https://github.com/espressif/esptool/wiki/Serial-Protocol:
    // The bootloader protocol uses SLIP framing. Each packet begin and end
    // with 0xC0, all occurrences of 0xC0 and 0xDB inside the packet are
    // replaced with 0xDB 0xDC and 0xDB 0xDD, respectively.
    QByteArray encoded;

    encoded.append((char)0xC0);

    for(char byte : data) {
        if(byte == ((char)0xC0)) {
            encoded.append((char)0xDB);
            encoded.append((char)0xDC);
        }
        else if(byte == ((char)0xDB)) {
            encoded.append((char)0xDB);
            encoded.append((char)0xDD);
        }
        else {
            encoded.append(byte);
        }
    }

    encoded.append((char)0xC0);

    return encoded;
}

QByteArray slipDecode(const QByteArray &data)
{
    if(!data.startsWith((char)0xC0)) {
        return QByteArray();
    }

    if(!data.endsWith((char)0xC0)) {
        return QByteArray();
    }

    QByteArray decoded = data.mid(1, data.length() - 2);

    decoded.replace(QByteArray().append(0xDB).append(0xDC),
                    QByteArray().append(0xC0));

    decoded.replace(QByteArray().append(0xDB).append(0xDD),
                    QByteArray().append(0xDB));

    return decoded;
}

SerialCommand readRegister(unsigned int address)
{
    QByteArray command;
    command.append((char)0x00);
    command.append(Opcode_ReadRegister);
    command.append(ByteArrayHelpers::uint16ToByteArrayLittle(4));
    command.append(QByteArray(4, 0x00));
    command.append(ByteArrayHelpers::uint32ToByteArrayLittle(address));

    QByteArray expectedResponse;
    expectedResponse.append(0x01);
    expectedResponse.append(Opcode_ReadRegister);
    expectedResponse.append(ByteArrayHelpers::uint16ToByteArrayLittle(6));
    expectedResponse.append(QByteArray(4,0x00));  // register data
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)0x00);

    QByteArray expectedResponseMask;
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append(QByteArray(2, 0xFF));
    expectedResponseMask.append(QByteArray(4,0x00));
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0x00);

    return SerialCommand("readRegister", command, expectedResponse, expectedResponseMask);
}

SerialCommand SyncFrame()
{
    QByteArray command;
    command.append((char)0x00);
    command.append(Opcode_SyncFrame);
    command.append(ByteArrayHelpers::uint16ToByteArrayLittle(36));
    command.append(QByteArray(4, 0x00));
    command.append((char)0x07);
    command.append((char)0x07);
    command.append((char)0x12);
    command.append((char)0x20);
    command.append(QByteArray(32, 0x55));

    QByteArray expectedResponse;
    expectedResponse.append(0x01);
    expectedResponse.append(Opcode_SyncFrame);
    expectedResponse.append(ByteArrayHelpers::uint16ToByteArrayLittle(2));
    expectedResponse.append((char)0x07);
    expectedResponse.append((char)0x07);
    expectedResponse.append((char)0x12);
    expectedResponse.append((char)0x20);
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)0x00);

    qDebug() << command.toHex();
    qDebug() << expectedResponse.toHex();

    command = slipEncode(command);
    expectedResponse = slipEncode(expectedResponse);
    expectedResponse = expectedResponse
            + expectedResponse
            + expectedResponse
            + expectedResponse
            + expectedResponse
            + expectedResponse
            + expectedResponse
            + expectedResponse;

    return SerialCommand("SyncFrame", command, expectedResponse, 200);
}

}
