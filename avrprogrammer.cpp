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

    QByteArray command;
    command.append('A');
    command.append((address >> 8) & 0xFF);
    command.append((address >> 0) & 0xFF);

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

        std::cout << "Chunk position: " << std::setfill('0') << std::setw(4) << std::hex << currentChunkPosition
                  << " size: " << std::setfill('0') << std::setw(4) << std::hex << currentChunkSize
                  << std::endl;

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

void AvrProgrammer::enterProgrammingMode() {

    if(!isConnected()) {
        std::cout << "not connected!" << std::endl;
        return;
    }

    if(!checkDeviceSignature()) {
        return;
    }

    QByteArray readFromFlash;
    int startAddress = 0;
    int lengthBytes = 28672; // 4k is reserved for the bootloader, so don't bother reading it.
    if(!readFlash(readFromFlash, startAddress, lengthBytes)) {
        return;
    }

    // TODO: Delete me.
    for(int i = 0; i < readFromFlash.length(); i++) {
        if(i%16 == 0) {
            std::cout << std::setfill('0') << std::setw(4) << std::hex << i << ": ";
        }
        std::cout << std::setfill('0') << std::setw(2) << std::hex << ((int)(readFromFlash.at(i) & 0xFF)) << " ";
        if(i%16 == 15) {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;

    std::cout << "is..." << std::endl;
    QByteArray sketch(PatternPlayerSketch,PATTERNPLAYER_LENGTH);
    std::cout << "OK!" << std::endl;

    if(!writeFlash(sketch,0)) {
        return;
    }

    if(!reset()) {
        return;
    }


    std::cout << "Great!" << std::endl;
}
