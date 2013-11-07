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

    serial = new QSerialPort(this);
    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));
}

void BlinkyTape::handleError(QSerialPort::SerialPortError error)
{
    // TODO: handle other error types?
    if (error == QSerialPort::ResourceError) {
        std::cout << "Critical error:" << serial->errorString().toStdString() << std::endl;
        close();
    }
}

void BlinkyTape::open(QSerialPortInfo info) {
    if(isConnected()) {
        return;
    }

    // TODO: Do something else if we can't open?
    serial->setPort(info);
//    serial->setBaudRate(QSerialPort::Baud115200);
    serial->open(QIODevice::ReadWrite);

    if(isConnected()) {
        emit(connectionStatusChanged(isConnected()));
    }
}

void BlinkyTape::close() {
    serial->close();

    emit(connectionStatusChanged(isConnected()));
}

bool BlinkyTape::isConnected() {
    return serial->isOpen();
}

void BlinkyTape::sendUpdate(QByteArray LedData)
{
    if(!isConnected()) {
        // TODO: Signal error?
        std::cout << "Strip not connected, not sending update!" << std::endl;
        return;
    }

    // TODO: Check if we can write to the device?

    if(LedData.length() != ledCount*3) {
        std::cout << "Length not correct, not sending update!" << std::endl;
        return;
    }

    // Trim anything that's 0xff
    for(int i = 0; i < LedData.length(); i++) {
        if(LedData[i] == (char)255) {
            LedData[i] = 254;
        }
    }

    // Append an 0xFF to signal the flip command
    LedData.append(0xFF);

    int writeLength = serial->write(LedData);
    if(writeLength != LedData.length()) {
        std::cout << "Error writing all the data out, expected:" << LedData.length()
                  << ", wrote: " << writeLength << std::endl;
    }
}

void BlinkyTape::uploadAnimation(QByteArray animation, int frameRate) {
    std::cout << "BlinkyTape::uploadAnimation: " << thread() << std::endl;

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
    QSerialPortInfo currentPort(*serial);

    // Now, set the currently connected tapes baud rate to 1200, then close it to
    // apply the 1200 baud reset
    // TODO: This seems to work on OS X, test on Windows and Linux
    std::cout << "Resetting" << std::endl;
    serial->setBaudRate(QSerialPort::Baud1200);
    serial->setSettingsRestoredOnClose(false);
    serial->close();
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
