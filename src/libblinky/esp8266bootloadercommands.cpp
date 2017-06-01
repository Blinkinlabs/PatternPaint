#include "esp8266bootloadercommands.h"
#include "bytearrayhelpers.h"

#include <QDebug>

namespace Esp8266BootloaderCommands {

unsigned char calculateChecksum(const QByteArray &data)
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

QByteArray buildCommand(Opcode opcode, const QByteArray &data, unsigned char checksum)
{
    QByteArray command;
    command.append((char)0x00);
    command.append(opcode);
    command.append(ByteArrayHelpers::uint16ToByteArrayLittle(data.length()));
    command.append(ByteArrayHelpers::uint32ToByteArrayLittle(checksum));
    command.append(data);

    return command;
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

SerialCommand flashDownloadStart(unsigned int totalSize,
                                 unsigned int numberOfBlocks,
                                 unsigned int blockSize,
                                 unsigned int offset)
{
    QByteArray data;
    data.append(ByteArrayHelpers::uint32ToByteArrayLittle(totalSize));
    data.append(ByteArrayHelpers::uint32ToByteArrayLittle(numberOfBlocks));
    data.append(ByteArrayHelpers::uint32ToByteArrayLittle(blockSize));
    data.append(ByteArrayHelpers::uint32ToByteArrayLittle(offset));

    QByteArray command = buildCommand(Opcode_FlashDownloadStart, data, calculateChecksum(data));

    QByteArray expectedResponse;
    expectedResponse.append(0x01);
    expectedResponse.append(Opcode_FlashDownloadStart);
    expectedResponse.append(ByteArrayHelpers::uint16ToByteArrayLittle(2));
    expectedResponse.append((char)0x07);
    expectedResponse.append((char)0x07);
    expectedResponse.append((char)0x12);
    expectedResponse.append((char)0x20);
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)0x00);

    return SerialCommand("flashDownloadStart",
                         slipEncode(command),
                         slipEncode(expectedResponse));
}

SerialCommand flashDownloadData(unsigned int sequence, QByteArray flashData)
{
    QByteArray data;
    data.append(ByteArrayHelpers::uint32ToByteArrayLittle(flashData.length()));
    data.append(ByteArrayHelpers::uint32ToByteArrayLittle(sequence));
    data.append(ByteArrayHelpers::uint32ToByteArrayLittle(0));  // Mystery value
    data.append(ByteArrayHelpers::uint32ToByteArrayLittle(0));  // Mystery value
    data.append(flashData);

    QByteArray command = buildCommand(Opcode_FlashDownloadData, data, calculateChecksum(flashData));

    QByteArray expectedResponse;
    expectedResponse.append(0x01);
    expectedResponse.append(Opcode_FlashDownloadData);
    expectedResponse.append(ByteArrayHelpers::uint16ToByteArrayLittle(2));
    expectedResponse.append((char)0x07);
    expectedResponse.append((char)0x07);
    expectedResponse.append((char)0x12);
    expectedResponse.append((char)0x20);
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)0x00);

    return SerialCommand("flashDownloadData",
                         slipEncode(command),
                         slipEncode(expectedResponse));
}

SerialCommand flashDownloadFinish(unsigned int rebootFlag)
{
    QByteArray data;
    data.append(ByteArrayHelpers::uint32ToByteArrayLittle(rebootFlag));

    QByteArray command = buildCommand(Opcode_FlashDownloadFinish, data, calculateChecksum(data));

    QByteArray expectedResponse;
    expectedResponse.append(0x01);
    expectedResponse.append(Opcode_FlashDownloadFinish);
    expectedResponse.append(ByteArrayHelpers::uint16ToByteArrayLittle(2));
    expectedResponse.append((char)0x07);
    expectedResponse.append((char)0x07);
    expectedResponse.append((char)0x12);
    expectedResponse.append((char)0x20);
    expectedResponse.append((char)0x01);  // TODO: Can we count on these?
    expectedResponse.append((char)0x06);  // TODO: Can we count on these?

    return SerialCommand("flashDownloadFinish",
                         slipEncode(command),
                         slipEncode(expectedResponse));
}

SerialCommand SyncFrame()
{
    QByteArray data;
    data.append((char)0x07);
    data.append((char)0x07);
    data.append((char)0x12);
    data.append((char)0x20);
    data.append(QByteArray(32, 0x55));

    QByteArray command = buildCommand(Opcode_SyncFrame, data, calculateChecksum(data));

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

    command = slipEncode(command);

    // TODO: Unclear why the esp responds with the same message repeated multiple times
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
