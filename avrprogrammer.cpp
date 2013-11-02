#include "avrprogrammer.h"
#include <iostream>

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
        std::cout << "Fuck, response didn't match!:" << response.at(0) << std::endl;
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

void AvrProgrammer::enterProgrammingMode() {
    if(!isConnected()) {
        std::cout << "not connected!" << std::endl;
        return;
    }

    if(!checkSoftwareIdentifier()) {
        return;
    }

    std::cout << "Great!" << std::endl;
}

void AvrProgrammer::leaveProgrammingMode() {

}
