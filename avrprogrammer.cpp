#include "avrprogrammer.h"
#include <iostream>
#include <iomanip>

#include "PatternPlayer_Sketch.h"

AvrProgrammer::AvrProgrammer()
{
}

bool AvrProgrammer::connect(QSerialPortInfo info) {
    // TODO: Refuse if we are already open?

    std::cout << info.manufacturer().toStdString() << std::endl;
    serial.setBaudRate(QSerialPort::Baud115200);  // TODO: Delete me
    serial.setPort(info);

    // TODO: Do something else if we can't open?
    return serial.open(QIODevice::ReadWrite);
}

void AvrProgrammer::disconnect() {
    if(!isConnected()) {
        return;
    }

    serial.close();
}

bool AvrProgrammer::isConnected() {
    return serial.isOpen();
}

bool AvrProgrammer::sendCommand(QByteArray command) {
    #define WRITE_TIMEOUT_MS 500

    // TODO: Check error? check connected??

    if(serial.write(command) != command.length()) {
        std::cout << "Error writing to device" << std::endl;
        return false;
    }

    if(serial.error() != QSerialPort::NoError) {
        std::cout << serial.errorString().toStdString() << std::endl;
        return false;
    }

    serial.waitForBytesWritten(WRITE_TIMEOUT_MS);
    if(serial.bytesToWrite() > 0) {
        std::cout << "Fuck, timeout on write!" << std:: endl;
        return false;
    }

    return true;
}

bool AvrProgrammer::readResponse(QByteArray& response, int expectedLength) {
    #define READ_TIMEOUT_MS 500

    response.clear();

    do {
        response += serial.read(expectedLength - response.length());
    }
    while ((response.length() < expectedLength) && serial.waitForReadyRead(READ_TIMEOUT_MS));

    if(response.length() != expectedLength) {
        std::cout << "Fuck, didnt get correct amout of data! got: " << response.length() << std::endl;
        return false;
    }

    if(serial.error() != QSerialPort::NoError) {
        std::cout << serial.errorString().toStdString() << std::endl;
        return false;
    }

    return true;
}

bool AvrProgrammer::checkResponse(QByteArray expectedResponse) {
    #define READ_TIMEOUT_MS 500

    QByteArray response = serial.readAll();
    while (serial.waitForReadyRead(READ_TIMEOUT_MS)) {
        response += serial.readAll();
    }

    if(response.length() != expectedResponse.length()) {
        std::cout << "Fuck, didnt get correct amout of data! got: " << response.length() << std::endl;
        return false;
    }

    if(serial.error() != QSerialPort::NoError) {
        std::cout << serial.errorString().toStdString() << std::endl;
        return false;
    }

    if( response != expectedResponse) {
        std::cout << "Fuck, response didn't match!:" << (int)response.at(0) << std::endl;
        return false;
    }

    return true;
}

bool AvrProgrammer::checkSoftwareIdentifier() {
    if(!sendCommand(QByteArray("S"))) {
        return false;
    }

    if(!checkResponse(QByteArray("CATERIN"))) {
        return false;
    }

    return true;
}

bool AvrProgrammer::checkDeviceSignature() {
    if(!sendCommand(QByteArray("s"))) {
        return false;
    }

    if(!checkResponse(QByteArray("\x87\x95\x1E"))) {
        return false;
    }

    return true;
}

bool AvrProgrammer::reset()
{
    if(!sendCommand(QByteArray("E"))) {
        return false;
    }

    if(!checkResponse(QByteArray("\r"))) {
        return false;
    }

    return true;
}

bool AvrProgrammer::setAddress(int address) {
    // TODO: Test if address > supported
    if(address & 0x01) {
        return false;
    }

    // Note that the address her is word aligned.
    QByteArray command;
    command.append('A');
    command.append((address >> 9) & 0xFF);
    command.append((address >> 1) & 0xFF);

    if(!sendCommand(command)) {
        return false;
    }

    if(!checkResponse(QByteArray("\r"))) {
        return false;
    }

    return true;
}

bool AvrProgrammer::getFlashPageSize(int& pageSizeBytes) {
    if(!sendCommand(QByteArray("b"))) {
        return false;
    }

    QByteArray buffer;
    if(!readResponse(buffer, 3)) {
        return false;
    }

    if(buffer.at(0) != 'Y') {
        return false;
    }

    pageSizeBytes = ((int)(buffer.at(1) & 0xFF) << 8)
                  + ((int)(buffer.at(2) & 0xFF)     );

    return true;
}

bool AvrProgrammer::readFlash(QByteArray& data, int startAddress, int lengthBytes) {
    // Set the address to read from
    // TODO: avoid the bootloader?
    if(!setAddress(startAddress)) {
        return false;
    }

    // Read the flash contents
    // Baller that we can just read the whole thing at once!
    QByteArray command;
    command.append('g'); // command: read memory
    command.append((lengthBytes >> 8) & 0xFF); // read size (high)
    command.append((lengthBytes)      & 0xFF); // read size (low)
    command.append('F'); // memory type: flash

    if(!sendCommand(command)) {
        return false;
    }

    // Read the block back, should be block size+1 bytes,
    QByteArray response;
    if(!readResponse(response,lengthBytes)) {
        return false;
    }

    // TODO: Delete me.
    for(int i = 0; i < response.length(); i++) {
        if(i%16 == 0) {
            std::cout << std::setfill('0') << std::setw(4) << std::hex << i << ": ";
        }
        std::cout << std::setfill('0') << std::setw(2) << std::hex << ((int)(response.at(i) & 0xFF)) << " ";
        if(i%16 == 15) {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;

    // Copy the response to return it to the calling function
    data = response;

    return true;
}

bool AvrProgrammer::writeFlash(QByteArray& data, int startAddress) {
    // First, get the block size TODO: We should know this ahead of time, we know
    // everything else already
    int pageSizeBytes;
    if(!getFlashPageSize(pageSizeBytes)) {
        return false;
    }

    // TODO: Is this necessicary?
    if(startAddress%pageSizeBytes) {
        std::cout << "Bad start address, must align with page boundary" << std::endl;
        return false;
    }

    if(data.length() <= 0) {
        std::cout << "No data to write" << std::endl;
        return false;
    }

    // Set the address to read from
    // TODO: avoid the bootloader?
    if(!setAddress(startAddress)) {
        return false;
    }

    // Write the data in page-sized chunks
    // TODO: Do we actually need to do chunks here?
    for(int currentChunkPosition = 0;
        currentChunkPosition < data.length();
        currentChunkPosition += pageSizeBytes) {

        int currentChunkSize = std::min(pageSizeBytes, data.length() - currentChunkPosition);

        std::cout << std::setfill('0') << std::setw(4) << std::hex << startAddress + currentChunkPosition;
        for(int i = 0; i < data.mid(currentChunkPosition,currentChunkSize).length(); i++) {
            if(i%16 == 0) {
                std::cout << ": ";
            }
            std::cout << std::setfill('0') << std::setw(2) << std::hex << ((int)(data.mid(currentChunkPosition,currentChunkSize).at(i) & 0xFF)) << " ";
            if(i%16 == 15) {
                std::cout << std::endl << "    ";
            }
        }
        std::cout << std::endl;

        QByteArray command;
        command.append('B'); // command: read memory
        command.append((currentChunkSize >> 8) & 0xFF); // read size (high)
        command.append((currentChunkSize)      & 0xFF); // read size (low)
        command.append('F'); // memory type: flash
        command.append(data.mid(currentChunkPosition,currentChunkSize));

        if(!sendCommand(command)) {
            return false;
        }

        // Read the block back, should be block size+1 bytes,
        QByteArray expectedResponse;
        expectedResponse.append('\r');
        if(!checkResponse(expectedResponse)) {
            return false;
        }
    }

    return true;
}

void AvrProgrammer::uploadAnimation(QByteArray animation, int frameRate) {

    if(!isConnected()) {
        std::cout << "not connected!" << std::endl;
        return;
    }

    if(!checkDeviceSignature()) {
        std::cout << "bad device signature!" << std::endl;
        return;
    }

    // First, convert the sketch binary data into a qbytearray
    QByteArray sketch(PatternPlayerSketch,PATTERNPLAYER_LENGTH);

    // Next, append the image data to it
    // TODO: Compress the animation
    sketch += animation;

    // The entire sketch must fit into the available memory, minus a single page
    // at the end of flash for the configuration header
    // TODO: Could save ~100 bytes if we let the sketch spill into the unused portion
    // of the header.
    if(sketch.length() > FLASH_MEMORY_AVAILABLE - FLASH_MEMORY_PAGE_SIZE) {
        std::cout << "sketch can't fit into memory!" << std::endl;
        return;
    }

    // Finally, write the metadata about the animation to the end of flash
    QByteArray metadata(FLASH_MEMORY_PAGE_SIZE, 0xFF); // TODO: Connect this to the block size
    metadata[metadata.length()-6] = (PATTERNPLAYER_LENGTH >> 8) & 0xFF;
    metadata[metadata.length()-5] = (PATTERNPLAYER_LENGTH     ) & 0xFF;
    metadata[metadata.length()-4] = ((animation.length()/3/60) >> 8) & 0xFF;
    metadata[metadata.length()-3] = ((animation.length()/3/60)     ) & 0xFF;
    metadata[metadata.length()-2] = (1000/frameRate >> 8) & 0xFF;
    metadata[metadata.length()-1] = (1000/frameRate     ) & 0xFF;

    std::cout << std::hex;
    std::cout <<  "Sketch size (bytes): 0x" << PATTERNPLAYER_LENGTH << std::endl;
    std::cout << "Animation size (bytes): 0x" << animation.length() << std::endl;
    std::cout << "Image size (bytes): 0x" << sketch.length() << std::endl;

    std::cout << "Animation address: 0x"
              << (int)metadata.at(124)
              << (int)metadata.at(125) << std::endl;
    std::cout << "Animation length: 0x"
              << (int)metadata.at(126)
              << (int)metadata.at(127) << std::endl;


    // Now, write the combined sketch and animation to flash
    if(!writeFlash(sketch,0)) {
        return;
    }

    // Now, write the animation metadata to the end of flash
    if(!writeFlash(metadata, 0x7000 - 0x80)) {
        return;
    }

    // TODO: Compare this to what we just wrote as a verification step.
    QByteArray readFromFlash;
    int startAddress = 0;
    int lengthBytes = FLASH_MEMORY_AVAILABLE; // 4k is reserved for the bootloader, so don't bother reading it.
    if(!readFlash(readFromFlash, startAddress, lengthBytes)) {
        return;
    }

    // Finally, reset the strip to start the animation.
    if(!reset()) {
        return;
    }


    std::cout << "Great!" << std::endl;
}
