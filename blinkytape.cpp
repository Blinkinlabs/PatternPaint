#include "avrprogrammer.h"
#include "blinkytape.h"

#include <iostream>
#include <termios.h>

QList<QSerialPortInfo> BlinkyTape::findBlinkyTapes() {
    QList<QSerialPortInfo> tapes;

    // TODO: Should we use VID/PID here instead of descriptions?
    // Maybe it's not the most important thing in the world.
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        // If the description is BlinkyTape, we're sure to have a match
        if(info.description().startsWith("BlinkyTape")) {
            tapes.push_back(info);
        }
        // If it's a leonardo, it /may/ be a BlinkyTape running a user sketch
        else if(info.description().startsWith("Arduino Leonardo")) {
            tapes.push_back(info);
        }
    }

    return tapes;
}

BlinkyTape::BlinkyTape(int ledCount_)
{
    ledCount = ledCount_;
}

bool BlinkyTape::connect(QSerialPortInfo info) {
    if(isConnected()) {
        return false;
    }
    serial.setPort(info);

    // TODO: Do something else if we can't open?
    serial.setBaudRate(QSerialPort::Baud115200);

    serial.open(QIODevice::ReadWrite);

    if(isConnected()) {
      emit(connectionStatusChanged(isConnected()));
    }

    return isConnected();
}

void BlinkyTape::disconnect() {
    if(isConnected()) {
        serial.close();
        emit(connectionStatusChanged(isConnected()));
    }
}

void BlinkyTape::uploadAnimation(QByteArray animation, int frameRate) {
    // TODO: Rethink the layout of this process, it's muddled.

    // We can't reset if we weren't already connected...
    if(!isConnected()) {
        return;
    }

    // First, record the currently available BlinkyTapes
    QList<QSerialPortInfo> preResetTapes = BlinkyTape::findBlinkyTapes();
    std::cout << "Current tapes: " << preResetTapes.length() << std::endl;
    for(int i = 0; i < preResetTapes.length(); i++) {
        std::cout << preResetTapes.at(i).portName().toStdString() << std::endl;
    }

    // Next, record the currently active port (TODO: do we care???)
    QSerialPortInfo currentPort(serial);

    // Now, set the currently connected tapes baud rate to 1200, then close it to
    // apply the 1200 baud reset
    // TODO: This seems to work on OS X, test on Windows and Linux
    std::cout << "Resetting" << std::endl;
    serial.setBaudRate(QSerialPort::Baud1200);
    serial.setSettingsRestoredOnClose(false);
    serial.close();
    emit(connectionStatusChanged(isConnected()));

    // Wait until we see a serial port drop out
    // TODO: Is this guarinteed to happen?
    #define RESET_TIMEOUT_MS 2000
    QDateTime time = QDateTime::currentDateTime();

    QList<QSerialPortInfo> midResetTapes;
    bool portDropped = false;
    while(!portDropped && time.msecsTo(QDateTime::currentDateTime()) < RESET_TIMEOUT_MS) {
        midResetTapes = BlinkyTape::findBlinkyTapes();
        if (midResetTapes.length() == preResetTapes.length() - 1) {
            portDropped = true;
            std::cout << "Port dropped, hooray!" << std::endl;
        }
    }
    if (portDropped == false) {
        // TODO: Timeout error waiting for port to dissappear.
        std::cout << "Timeout waiting for port to disappear..." << std::endl;
        return;
    }

    // Wait until we see a serial port appear again.
    QList<QSerialPortInfo> postResetTapes;
    bool portAdded = false;
    while(!portAdded && time.msecsTo(QDateTime::currentDateTime()) < RESET_TIMEOUT_MS) {
        postResetTapes = BlinkyTape::findBlinkyTapes();
        if (postResetTapes.length() == midResetTapes.length() + 1) {
//        if (postResetTapes.length() > 0) {
            portAdded = true;
            std::cout << "Port added, hooray!" << std::endl;
        }
    }
    if(!portAdded) {
        std::cout << "Timeout waiting for port to appear..." << std::endl;
        return;
    }

    // Remove all of the tapes that are in the mid-reset list from the post-reset list
    std::cout << "Post-reset tapes: " << postResetTapes.length() << std::endl;
    for(int i = 0; i < midResetTapes.length(); i++) {
        for(int j = 0; j < postResetTapes.length(); j++) {
            if(midResetTapes.at(i).portName() == postResetTapes.at(j).portName()) {
                postResetTapes.removeAt(j);
                break;
            }
        }
    }

    // Now, we should only have one new port.
    if(postResetTapes.length() != 1) {
        std::cout << "Too many or two few ports, something went wrong??" << std::endl;
    }

    std::cout << "Bootloader waiting on: " << postResetTapes.at(0).portName().toStdString() << std::endl;

    AvrProgrammer programmer;
    programmer.connect(postResetTapes.at(0));
    programmer.uploadAnimation(animation, frameRate);
}

bool BlinkyTape::isConnected() {
    return serial.isOpen();
}

void BlinkyTape::sendUpdate(QByteArray LedData)
{
    if(!isConnected()) {
        // TODO: Signal error?
        return;
    }

    // Kill anything that's 0xff
    // TODO: Don't do this in-place
    for(int i = 0; i < LedData.length(); i++) {
        if(LedData[i] == (char)255) {
            LedData[i] = 254;
        }
    }

    // Chunk it out
    int CHUNK_SIZE = 600;
    for(int p = 0; p < ledCount * 3; p += CHUNK_SIZE) {
        int length = 0;
        if(p + CHUNK_SIZE < LedData.length()) {
            // send a whole chunk
            length = CHUNK_SIZE;
        }
        else {
            // send a partial chunk
            length = LedData.length() - p;
        }

        QByteArray chunk(length, 0);
        for(int i = 0; i < length; i++) {
            chunk[i] = LedData[p + i];
        }

        int written = serial.write(chunk);
        // if we didn't write everything, save it for later.
        if(written == -1) {
            exit(100);
            p -= CHUNK_SIZE;
        }
        else if(written != length) {
            exit(101);
            p-= length - written;
        }
//        std::cout << "waiting for write" << std::endl;
        serial.flush();
//        std::cout << "done waiting for write" << std::endl;
    }

    // Then send the flip command
    QByteArray data(3, 0);
    data[0] = 0xff;
    data[1] = 0xff;
    data[2] = 0xff;

//    std::cout << "writing end" << std::endl;
    serial.write(data);
//    std::cout << "waiting for end" << std::endl;
    serial.flush();
//    std::cout << "done waiting for end" << std::endl;
}
