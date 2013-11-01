#include "avrprogrammer.h"
#include <iostream>

AvrProgrammer::AvrProgrammer()
{
}

bool AvrProgrammer::connect(QSerialPortInfo info) {
    // TODO: Refuse if we are already open?

    serial.setPort(info);

    // TODO: Do something else if we can't open?
    return serial.open(QIODevice::ReadWrite);
}

void AvrProgrammer::disconnect() {
    if(isConnected()) {
        serial.close();
    }
}

bool AvrProgrammer::isConnected() {
    return serial.isOpen();
}

void AvrProgrammer::enterProgrammingMode() {
    if(!isConnected()) {
        std::cout << "not connected!" << std::endl;
        return;
    }

    QByteArray buffer;
    buffer.append('Z');
    buffer.append('\r');
    buffer.append('Z');
    buffer.append('\r');
    buffer.append('Z');
    buffer.append('\r');
    buffer.append('Z');
    buffer.append('\r');
    buffer.append('Z');
    buffer.append('\r');
    buffer.append('Z');
    buffer.append('\r');

    std::cout << buffer.length() << std::endl;

    serial.clearError();
    if(serial.write(buffer) != buffer.length()) {
        std::cout << "Error writing to device" << std::endl;
    }

    if(serial.error() != QSerialPort::NoError) {
        std::cout << serial.errorString().toStdString() << std::endl;
    }

    serial.waitForBytesWritten(500);
    if(serial.bytesToWrite() > 0) {
        std::cout << "Fuck, timeout on write!" << std:: endl;
        return;
    }

    #define READ_TIMEOUT_MS 500
    QDateTime time = QDateTime::currentDateTime();

    int expectedDataLen = 1;

    QByteArray returnData;
    while(returnData.length() != expectedDataLen && time.msecsTo(QDateTime::currentDateTime()) < READ_TIMEOUT_MS) {
        returnData += serial.read(1);
    }
    if(returnData.length() != expectedDataLen) {
        std::cout << "Fuck, didnt get correct amout of data! got: " << returnData.length() << std::endl;
        return;
    }

    if( returnData.at(0) != '\r') {
        // TODO: Return code invalid, bail!
        std::cout << "Fuck, didn't get a CR back, got:" << (int)returnData.at(0) << std::endl;
    }
}

void AvrProgrammer::leaveProgrammingMode() {

}
