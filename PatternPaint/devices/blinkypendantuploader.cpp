#include "blinkypendantuploader.h"

BlinkyPendantUploader::BlinkyPendantUploader(QObject *parent) :
    PatternUploader(parent)
{
    connect(&programmer,SIGNAL(error(QString)),
            this,SLOT(handleProgrammerError(QString)));
    connect(&programmer,SIGNAL(commandFinished(QString,QByteArray)),
            this,SLOT(handleProgrammerCommandFinished(QString,QByteArray)));
}

bool BlinkyPendantUploader::startUpload(BlinkyController& controller, std::vector<Pattern> patterns)
{
    // TODO: push the image conversions into here so they are less awkward.

//    // Make sure we have an image compatible with the BlinkyPendant
//    QImage image = patterns.at(0).image;
//    QImage croppedImage;
//    croppedImage = image.copy( 0, 0, image.width(), 10);

//    Pattern pattern(croppedImage, 0, Pattern::RGB24, Pattern::RGB);

//    // Create the data structure to write to the device memory
//    QByteArray data;
//    data.append((char)0x13);    // header
//    data.append((char)0x37);
//    data.append((char)pattern.frameCount);  // frame count
//    data += pattern.data;       // image data (RGB24, uncompressed)

    // TODO: upgrade firmware for v1 devices?


    // Create the data structure to write to the device memory
    // Animation table
    QByteArray headerData;
    QByteArray patternData;

    headerData.append((char)0x31);    // header
    headerData.append((char)0x23);
    headerData.append((char)patterns.size()); // Number of patterns in the table
    headerData.append((char)10);            // Number of LEDs in the pattern

    for(std::vector<Pattern>::iterator pattern = patterns.begin();
        pattern != patterns.end();
        ++pattern) {

        // Make sure we have an image compatible with the BlinkyPendant
        QImage image = pattern->image;
        QImage croppedImage;
        croppedImage = image.copy( 0, 0, image.width(), 10);
        Pattern croppedPattern(croppedImage, 0, Pattern::RGB24, Pattern::RGB);

        // Animation entry
        headerData.append((char)0);             // Encoding type (1 byte) (RGB24, uncompressed) (TODO)
        headerData.append((char)((patternData.length() >> 24) &0xFF)); // Data offset (4 bytes)
        headerData.append((char)((patternData.length() >> 16) &0xFF));
        headerData.append((char)((patternData.length() >>  8) &0xFF));
        headerData.append((char)((patternData.length() >>  0) &0xFF));
        headerData.append((char)((croppedPattern.frameCount >> 8)&0xFF));   // Frame count (2 bytes)
        headerData.append((char)((croppedPattern.frameCount     )&0xFF));
        headerData.append((char)0);             // Frame delay (2 bytes) TODO
        headerData.append((char)0);

        // Make sure we have an image compatible with the BlinkyPendant
        patternData += croppedPattern.data;       // image data (RGB24, uncompressed)
    }

    headerData += patternData;

    // TODO: Check if the data can fit in the device memory

    // Set up the programmer using the serial descriptor, and close the tape connection
    QSerialPortInfo portInfo;
    if(!controller.getPortInfo(portInfo)) {
        errorString = "Couln't get port information!";
        return false;
    }
    controller.close();
    programmer.open(portInfo);

    setProgress(0);
    // TODO: Calculate this based on feedback from the programmer.
    setMaxProgress(10);

    // Queue the following commands:
    // 1. start write
    programmer.startWrite();

    // 2-n. write data (aligned to 1024-byte sectors, 64 bytes at a time)
    programmer.writeData(headerData);

    // n+1 stop write
    programmer.stopWrite();

    return true;
}

bool BlinkyPendantUploader::upgradeFirmware(BlinkyController& controller)
{
    Q_UNUSED(controller);

    errorString = "Firmware update not currently supported for BlinkyPendant!";
    return false;
}

QString BlinkyPendantUploader::getErrorString() const
{
    return errorString;
}

void BlinkyPendantUploader::handleProgrammerError(QString error) {
    qCritical() << error;

    if(programmer.isConnected()) {
        programmer.close();
    }

    emit(finished(false));
}

void BlinkyPendantUploader::handleProgrammerCommandFinished(QString command, QByteArray returnData) {
    Q_UNUSED(command);
    Q_UNUSED(returnData);

    qDebug() << "Command finished:" << command;
    setProgress(progress + 1);

    // TODO: Let the receiver handle this instead.
    if(command == "stopWrite") {
        programmer.close();
        emit(finished(true));
    }
}

void BlinkyPendantUploader::setProgress(int newProgress) {
    progress = newProgress;
    emit(progressChanged(progress));
}

void BlinkyPendantUploader::setMaxProgress(int newMaxProgress) {
    emit(maxProgressChanged(newMaxProgress));
}
