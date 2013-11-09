#include "avrprogrammer.h"
#include "blinkytape.h"
#include <QDebug>


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

BlinkyTape::BlinkyTape(QObject *parent, int ledCount_) :
    QObject(parent)
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
        qCritical() << serial->errorString();
        close();
    }
}

void BlinkyTape::open(QSerialPortInfo info) {
    if(isConnected()) {
        qDebug() << "Already connected";
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
        qCritical() << "Strip not connected, not sending update!";
        return;
    }

    // TODO: Check if we can write to the device?

    if(LedData.length() != ledCount*3) {
        qCritical() << "Length not correct, not sending update!";
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
        qCritical() << "Error writing all the data out, expected:" << LedData.length()
                    << ", wrote:" << writeLength;
    }
}

bool BlinkyTape::getPortInfo(QSerialPortInfo& info)
{
    if(!isConnected()) {
        return false;
    }

    info = QSerialPortInfo(*serial);
    return true;
}

void BlinkyTape::uploadAnimation(QByteArray animation, int frameRate) {
//    // TODO: Rethink the layout of this process, it's muddled.
//    // TODO: Only write the sketch portion if we absolutely have to, to lower the risk
//    // that we trash the firmware if there is a problem.

//    // We can't reset if we weren't already connected...
//    if(!isConnected()) {
//        return;
//    }


///// Create the compressed image and check if it will fit into the device memory
//    // First, convert the sketch binary data into a qbytearray
//    QByteArray sketch(PatternPlayerSketch,PATTERNPLAYER_LENGTH);

//    // Next, append the image data to it
//    // TODO: Compress the animation
//    sketch += animation;

//    // Finally, write the metadata about the animation to the end of flash
//    QByteArray metadata(FLASH_MEMORY_PAGE_SIZE, 0xFF); // TODO: Connect this to the block size
//    metadata[metadata.length()-6] = (PATTERNPLAYER_LENGTH >> 8) & 0xFF;
//    metadata[metadata.length()-5] = (PATTERNPLAYER_LENGTH     ) & 0xFF;
//    metadata[metadata.length()-4] = ((animation.length()/3/60) >> 8) & 0xFF;
//    metadata[metadata.length()-3] = ((animation.length()/3/60)     ) & 0xFF;
//    metadata[metadata.length()-2] = (1000/frameRate >> 8) & 0xFF;
//    metadata[metadata.length()-1] = (1000/frameRate     ) & 0xFF;

//    std::cout << std::hex;
//    std::cout <<  "Sketch size (bytes): 0x" << PATTERNPLAYER_LENGTH << std::endl;
//    std::cout << "Animation size (bytes): 0x" << animation.length() << std::endl;
//    std::cout << "Image size (bytes): 0x" << sketch.length() << std::endl;
//    std::cout << "Metadata size (bytes): 0x" << metadata.length() << std::endl;

//    std::cout << "Animation address: 0x"
//              << (int)metadata.at(122)
//              << (int)metadata.at(123) << std::endl;
//    std::cout << "Animation length: 0x"
//              << (int)metadata.at(124)
//              << (int)metadata.at(125) << std::endl;
//    std::cout << "Animation Delay: 0x"
//              << (int)metadata.at(126)
//              << (int)metadata.at(127) << std::endl;

//    // The entire sketch must fit into the available memory, minus a single page
//    // at the end of flash for the configuration header
//    // TODO: Could save ~100 bytes if we let the sketch spill into the unused portion
//    // of the header.
//    if(sketch.length() + metadata.length() > FLASH_MEMORY_AVAILABLE) {
//        std::cout << "sketch can't fit into memory!" << std::endl;
//        return;
//    }

///// Attempt to reset the strip using the 1200 baud rate method, and identify the newly connected bootloader

//    // First, record the currently available BlinkyTapes
//    QList<QSerialPortInfo> preResetTapes = BlinkyTape::findBlinkyTapes();
//    std::cout << "Current tapes: " << preResetTapes.length() << std::endl;
//    for(int i = 0; i < preResetTapes.length(); i++) {
//        std::cout << preResetTapes.at(i).portName().toStdString() << std::endl;
//    }

//    // Next, record the currently active port (TODO: do we care???)
//    QSerialPortInfo currentPort(*serial);

//    // Now, set the currently connected tapes baud rate to 1200, then close it to
//    // apply the 1200 baud reset
//    // TODO: This seems to work on OS X, test on Windows and Linux
//    std::cout << "Resetting" << std::endl;
//    serial->setBaudRate(QSerialPort::Baud1200);
//    serial->setSettingsRestoredOnClose(false);
//    serial->close();
//    emit(connectionStatusChanged(isConnected()));

//    // Wait until we see a serial port drop out
//    // TODO: Is this guarinteed to happen?
//    #define RESET_TIMEOUT_MS 2000
//    QDateTime time = QDateTime::currentDateTime();

//    QList<QSerialPortInfo> midResetTapes;
//    bool portDropped = false;
//    while(!portDropped && time.msecsTo(QDateTime::currentDateTime()) < RESET_TIMEOUT_MS) {
//        midResetTapes = BlinkyTape::findBlinkyTapes();
//        if (midResetTapes.length() == preResetTapes.length() - 1) {
//            portDropped = true;
//            std::cout << "Port dropped, hooray!" << std::endl;
//        }
//    }
//    if (portDropped == false) {
//        // TODO: Timeout error waiting for port to dissappear.
//        std::cout << "Timeout waiting for port to disappear..." << std::endl;
//        return;
//    }

//    // Wait until we see a serial port appear again.
//    QList<QSerialPortInfo> postResetTapes;
//    bool portAdded = false;
//    while(!portAdded && time.msecsTo(QDateTime::currentDateTime()) < RESET_TIMEOUT_MS) {
//        postResetTapes = BlinkyTape::findBlinkyTapes();
//        if (postResetTapes.length() == midResetTapes.length() + 1) {
////        if (postResetTapes.length() > 0) {
//            portAdded = true;
//            std::cout << "Port added, hooray!" << std::endl;
//        }
//    }
//    if(!portAdded) {
//        std::cout << "Timeout waiting for port to appear..." << std::endl;
//        return;
//    }

//    // Remove all of the tapes that are in the mid-reset list from the post-reset list
//    std::cout << "Post-reset tapes: " << postResetTapes.length() << std::endl;
//    for(int i = 0; i < midResetTapes.length(); i++) {
//        for(int j = 0; j < postResetTapes.length(); j++) {
//            if(midResetTapes.at(i).portName() == postResetTapes.at(j).portName()) {
//                postResetTapes.removeAt(j);
//                break;
//            }
//        }
//    }

//    // Now, we should only have one new port.
//    if(postResetTapes.length() != 1) {
//        std::cout << "Too many or two few ports, something went wrong??" << std::endl;
//    }

//    std::cout << "Bootloader waiting on: " << postResetTapes.at(0).portName().toStdString() << std::endl;


///// Connect to the programmer and write the data out

//    // Try to create a new programmer by connecting to the port
//    AvrProgrammer programmer;
//    programmer.connect(postResetTapes.at(0));

//    if(!programmer.isConnected()) {
//        std::cout << "not connected!" << std::endl;
//        return;
//    }

//    if(!programmer.checkDeviceSignature()) {
//        std::cout << "bad device signature!" << std::endl;
//        return;
//    }

//    // Now, write the combined sketch and animation to flash
//    if(!programmer.writeFlash(sketch,0)) {
//        return;
//    }

//    // Now, write the animation metadata to the end of flash
//    if(!programmer.writeFlash(metadata, 0x7000 - 0x80)) {
//        return;
//    }

///// Verify that the data was written correctly and reset.

//    // TODO: Compare this to what we just wrote as a verification step.
//    QByteArray readFromFlash;
//    int startAddress = 0;
//    int lengthBytes = FLASH_MEMORY_AVAILABLE; // 4k is reserved for the bootloader, so don't bother reading it.
//    if(!programmer.readFlash(readFromFlash, startAddress, lengthBytes)) {
//        return;
//    }

//    // Finally, reset the strip to start the animation.
//    if(!programmer.reset()) {
//        return;
//    }
    //    std::cout << "Great!" << std::endl;
}

void BlinkyTape::reset()
{
    if(!isConnected()) {
        return;
    }

    qDebug() << "Resetting";
    serial->setBaudRate(QSerialPort::Baud1200);
    serial->setSettingsRestoredOnClose(false);
    serial->close();
    emit(connectionStatusChanged(isConnected()));
}
