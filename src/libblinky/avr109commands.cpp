#include "avr109commands.h"
#include "bytearrayhelpers.h"
#include "serialcommandqueue.h"

#include <QDebug>

#define ATMEGA32_SIGNATURE "\x87\x95\x1E"   // Chip signature for ATMEGA32

#define FLASH_PAGE_SIZE_BYTES 128 // This could be polled using the 'b' command

#define EEPROM_CHUNK_SIZE_BYTES 32 // EEPROM writes are slow, so chunk them into smaller commands



namespace Avr109Commands {

// Send the command to probe for the device signature, and register the expected response
SerialCommand checkDeviceSignature()
{
    // TODO: Dont' hard-code this to the atmega32 response?
    return SerialCommand("checkDeviceSignature", QByteArray("s"), QByteArray(ATMEGA32_SIGNATURE));
}

SerialCommand reset()
{
    return SerialCommand("reset", QByteArray("E"), QByteArray("\r"));
}

SerialCommand chipErase()
{
    SerialCommand command("chipErase", QByteArray("e"), QByteArray("\r"));
    command.timeout = 10000;

    return command;
}

SerialCommand setAddress(unsigned int address)
{
    // Note that the address is word defined for flash, but byte defined for EEPROM.
    QByteArray command;
    command.append('A');
    command += uint16ToByteArrayBig(address);

    QByteArray response;
    response.append('\r');

    return SerialCommand("setAddress", command, response);
}

SerialCommand writeFlashPage(const QByteArray &data)
{
    // Writes need to be word aligned. Pad them if required.
    QByteArray paddedData(data);
    if(paddedData.length() % 2 == 1) {
        paddedData.append(static_cast<unsigned char>(0xFF));
    }

    QByteArray command;
    command.append('B'); // command: write memory
    command += uint16ToByteArrayBig(paddedData.count());  // write size (bytes)
    command.append('F'); // memory type: flash
    command += paddedData;

    QByteArray response;
    response.append('\r');

    return SerialCommand("writeFlashPage", command, response);
}

SerialCommand verifyFlashPage(const QByteArray &data)
{
    // Writes need to be word aligned. Pad them if required.
    QByteArray paddedData(data);
    if(paddedData.length() % 2 == 1) {
        paddedData.append(static_cast<unsigned char>(0xFF));
    }

    QByteArray command;
    command.append('g'); // command: verify memory
    command += uint16ToByteArrayBig(paddedData.count()); // read size (bytes)
    command.append('F'); // memory type: flash

    QByteArray response;
    response += paddedData;

    return SerialCommand("verifyFlashPage", command, response);
}

SerialCommand writeEepromBlock(const QByteArray &data)
{
    QByteArray command;
    command.append('B'); // command: write memory
    command += uint16ToByteArrayBig(data.count());  // write size (bytes)
    command.append('E'); // memory type: eeprom
    command += data;

    QByteArray response;
    response.append('\r');

    return SerialCommand("writeEepromBlock", command, response);
}

QList<SerialCommand> writeFlash(const QByteArray &data, unsigned int startAddress)
{
    QList<SerialCommand> commands;

    if (startAddress%FLASH_PAGE_SIZE_BYTES) {
        qCritical() << "Bad start address, must align with page boundary";
        return commands;
    }

    QList<QByteArray> chunks = chunkData(data, FLASH_PAGE_SIZE_BYTES);

    for(int chunkIndex = 0; chunkIndex < chunks.length(); chunkIndex++) {
        // Write the word address for Flash writes
        int currentAddress = startAddress + chunkIndex*FLASH_PAGE_SIZE_BYTES;
        commands.append(setAddress(currentAddress >> 1));

        // Then write the chunk
        commands.append(writeFlashPage(chunks.at(chunkIndex)));
    }

    return commands;
}

QList<SerialCommand> verifyFlash(const QByteArray &data, unsigned int startAddress)
{
    QList<SerialCommand> commands;

    if (startAddress%2 != 0) {
        qCritical() << "Bad start address, must align with word boundary";
        return commands;
    }

    QList<QByteArray> chunks = chunkData(data, FLASH_PAGE_SIZE_BYTES);

    for(int chunkIndex = 0; chunkIndex < chunks.length(); chunkIndex++) {
        // Write the word address for Flash writes
        int currentAddress = startAddress + chunkIndex*FLASH_PAGE_SIZE_BYTES;
        commands.append(setAddress(currentAddress >> 1));

        // Then write the chunk
        commands.append(verifyFlashPage(chunks.at(chunkIndex)));
    }

    return commands;
}

QList<SerialCommand> writeEeprom(const QByteArray &data, unsigned int startAddress)
{
    QList<SerialCommand> commands;

    // TODO: Check that we fit into available eprom.

    // Write the byte address for EEPROM writes
    commands.append(setAddress(startAddress));

    // Write the data in small chunks, so that the write doesn't time out.
    for (QByteArray chunk : chunkData(data, EEPROM_CHUNK_SIZE_BYTES))
        commands.append(writeEepromBlock(chunk));

    return commands;
}

} // namespace
