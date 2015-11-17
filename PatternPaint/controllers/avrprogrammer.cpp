#include "avrprogrammer.h"

#include <QDebug>

#define PAGE_SIZE_BYTES 128

AvrProgrammer::AvrProgrammer(QObject *parent) :
    SerialCommandQueue(parent)
{
}

// Send the command to probe for the device signature, and register the expected response
void AvrProgrammer::checkDeviceSignature() {
    queueCommand("checkDeviceSignature",QByteArray("s"), QByteArray("\x87\x95\x1E"));
}

void AvrProgrammer::reset()
{
    queueCommand("reset",QByteArray("E"), QByteArray("\r"));
}

void AvrProgrammer::setAddress(int address) {
    // TODO: Test if address > supported
    if(address & 0x01) {
        // TODO: report error
        qCritical() << "Address out of bounds";
    }

    // Note that the address her is word aligned.
    QByteArray command;
    command.append('A');
    command.append((address >> 9) & 0xFF);
    command.append((address >> 1) & 0xFF);

    queueCommand("setAddress", command, QByteArray("\r"));
}

void AvrProgrammer::readFlash(int startAddress, int lengthBytes) {
    // Set the address to read from
    // TODO: avoid the bootloader?
    setAddress(startAddress);

    // Read the flash contents
    // Baller that we can just read the whole thing at once!
    QByteArray command;
    command.append('g'); // command: read memory
    command.append((lengthBytes >> 8) & 0xFF); // read size (high)
    command.append((lengthBytes)      & 0xFF); // read size (low)
    command.append('F'); // memory type: flash

    // Note: only the length matters for the response, the response data
    // will be ignored.
    queueCommand("readFlash",command,QByteArray(lengthBytes + 1, '\r'));
}

void AvrProgrammer::writeFlash(QByteArray& data, int startAddress) {
    if(startAddress%PAGE_SIZE_BYTES) {
        qCritical() << "Bad start address, must align with page boundary";
        return;
    }

    if(data.length() == 0) {
        qCritical() << "No data to write";
        return;
    }

    // Pad the data length to an even number, since we can only write word-sized chunks
    if(data.length() % 2 == 1) {
        qDebug() << "Data length falls on a byte boundary, padding to a word boundary";
        data.append(0xff);
    }

    // TODO: Check that we fit into available memory.

    setAddress(startAddress);

    // Write the data in page-sized chunks
    for(int currentChunkPosition = 0;
        currentChunkPosition < data.length();
        currentChunkPosition += PAGE_SIZE_BYTES) {

        int currentChunkSize = std::min(PAGE_SIZE_BYTES, data.length() - currentChunkPosition);

        QByteArray command;
        command.append('B'); // command: write memory
        command.append((currentChunkSize >> 8) & 0xFF); // read size (high)
        command.append((currentChunkSize)      & 0xFF); // read size (low)
        command.append('F'); // memory type: flash
        command.append(data.mid(currentChunkPosition,currentChunkSize));

        queueCommand("writeFlash", command, QByteArray("\r"));
    }
}
