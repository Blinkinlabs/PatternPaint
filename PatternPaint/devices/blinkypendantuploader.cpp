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
    // Make sure we have an image compatible with the BlinkyPendant
    QImage image = patterns.at(0).image;
    QImage croppedImage;
    croppedImage = image.copy( 0, 0, image.width(), 10);
    Pattern pattern(croppedImage, 0, Pattern::RGB24, Pattern::RGB);

    // Create the data structure to write to the device memory
    QByteArray data;
    data.append((char)0x13);    // header
    data.append((char)0x37);
    data.append((char)pattern.frameCount);  // frame count
    data += pattern.data;       // image data (RGB24, uncompressed)

    // Check if the data can fit in the device memory
    // TODO

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
    programmer.writeData(data);

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
