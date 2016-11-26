#include "avr109commands.h"
#include "serialcommandqueue.h"

#include <QDebug>

#define FLASH_PAGE_SIZE_BYTES 128 // This could be polled using the 'b' command

#define EEPROM_CHUNK_SIZE_BYTES 32 // EEPROM writes are slow, so chunk them into smaller commands

namespace Avr109Commands {
// Send the command to probe for the device signature, and register the expected response
SerialCommand checkDeviceSignature()
{
    return SerialCommand("checkDeviceSignature", QByteArray("s"), QByteArray("\x87\x95\x1E"));
}

SerialCommand reset()
{
    return SerialCommand("reset", QByteArray('E'), QByteArray('\r'));
}

SerialCommand setAddress(int address)
{
    // Note that the address is word defined for flash, but byte defined for EEPROM.
    QByteArray command;
    command.append('A');
    command.append((address >> 8) & 0xFF);
    command.append((address >> 0) & 0xFF);

    QByteArray response;
    response.append('\r');

    return SerialCommand("setAddress", command, response);
}

// SerialCommand readFlash(int startAddress, int lengthBytes)
// {
//// Set the address to read from
//// TODO: avoid the bootloader?
// setAddress(startAddress);

//// Read the flash contents
//// Baller that we can just read the whole thing at once!
// QByteArray command;
// command.append('g'); // command: read memory
// command.append((lengthBytes >> 8) & 0xFF); // read size (high)
// command.append((lengthBytes)      & 0xFF); // read size (low)
// command.append('F'); // memory type: flash

// QByteArray response;
// QByteArray responseMask;
// for(int i = 0; i < lengthBytes; i++) {
// response.append((char)0x00);
// responseMask.append((char)0x00);
// }
// response.append('\r');
// responseMask.append((char)0xFF);

//// Note: only the length matters for the response, the response data
//// will be ignored.
// return SerialCommand("readFlash", command, response, responseMask);
// }

SerialCommand writeFlashBlock(QByteArray data)
{
    QByteArray command;
    command.append('B'); // command: write memory
    command.append((data.count() >> 8) & 0xFF); // write size (high)
    command.append((data.count())      & 0xFF); // write size (low)
    command.append('F'); // memory type: flash
    command += data;

    QByteArray response;
    response.append('\r');

    return SerialCommand("writeFlashBlock", command, response);
}

SerialCommand writeEepromBlock(QByteArray data)
{
    QByteArray command;
    command.append('B'); // command: write memory
    command.append((data.count() >> 8) & 0xFF); // write size (high)
    command.append((data.count())      & 0xFF); // write size (low)
    command.append('E'); // memory type: eeprom
    command += data;

    QByteArray response;
    response.append('\r');

    return SerialCommand("writeEepromBlock", command, response);
}

QList<SerialCommand> writeFlash(QByteArray &data, int startAddress)
{
    QList<SerialCommand> commands;

    if (startAddress%FLASH_PAGE_SIZE_BYTES) {
        qCritical() << "Bad start address, must align with page boundary";
        return commands;
    }

    if (data.length() == 0) {
        qCritical() << "No data to write";
        return commands;
    }

    // Pad the data length to an even number, since we can only write word-sized chunks
    if (data.length() % 2 == 1) {
        qDebug() << "Data length falls on a byte boundary, padding to a word boundary";
        data.append(0xff);
    }

    // TODO: Check that we fit into available memory.

    // Write the word address for Flash writes
    commands.append(setAddress(startAddress >> 1));

    // Write the data in page-sized chunks
    for (int currentChunkPosition = 0;
         currentChunkPosition < data.length();
         currentChunkPosition += FLASH_PAGE_SIZE_BYTES) {
        int currentChunkSize = std::min(FLASH_PAGE_SIZE_BYTES, data.length() - currentChunkPosition);

        commands.append(writeFlashBlock(data.mid(currentChunkPosition, currentChunkSize)));
    }

    return commands;
}

QList<SerialCommand> writeEeprom(QByteArray &data, int startAddress)
{
    QList<SerialCommand> commands;

    if (data.length() == 0) {
        qCritical() << "No data to write";
        return commands;
    }

    // TODO: Check that we fit into available eprom.

    // Write the byte address for EEPROM writes
    commands.append(setAddress(startAddress));

    // Write the data in small chunks, so that the write doesn't time out.
    for (int currentChunkPosition = 0;
         currentChunkPosition < data.length();
         currentChunkPosition += EEPROM_CHUNK_SIZE_BYTES) {
        int currentChunkSize = std::min(EEPROM_CHUNK_SIZE_BYTES, data.length() - currentChunkPosition);

        commands.append(writeEepromBlock(data.mid(currentChunkPosition, currentChunkSize)));
    }

    return commands;
}
}
