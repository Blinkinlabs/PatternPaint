#include "blinkypendantuploader.h"

#include "usbutils.h"
#include "blinkycontroller.h"
#include "blinkypendantcommands.h"
#include "bytearrayhelpers.h"

#define BLINKY_PENDANT_VERSION_1 256
#define BLINKY_PENDANT_VERSION_2 512

BlinkyPendantUploader::BlinkyPendantUploader(QObject *parent) :
    BlinkyUploader(parent)
{
    connect(&commandQueue, SIGNAL(error(QString)),
            this, SLOT(handleError(QString)));
    connect(&commandQueue, SIGNAL(commandFinished(QString, QByteArray)),
            this, SLOT(handleCommandFinished(QString, QByteArray)));
}

bool BlinkyPendantUploader::storePatterns(BlinkyController &controller,
                                        QList<PatternWriter> &patternWriters)
{
    // TODO: push the image conversions into here so they are less awkward.
    #define PIXEL_COUNT 10

    // Probe for the blinkypendant version
    // TODO: Update the firmware first!
    QSerialPortInfo portInfo;
    if (!controller.getPortInfo(portInfo)) {
        errorString = "Couln't get port information!";
        return false;
    }

    int version = getVersionForDevice(portInfo.vendorIdentifier(),
                                      portInfo.productIdentifier());

    QByteArray data;
    if (version == BLINKY_PENDANT_VERSION_1) {
        qDebug() << "Using version 1 upload mechanism, please update firmware!";

        // Make sure we have an image compatible with the BlinkyPendant
        if (patternWriters.front().getLedCount() != 10) {
            errorString = "Wrong pattern size- must be 10 pixels high!";
            return false;
        }
        if (patternWriters.front().getEncoding() != PatternWriter::RGB24) {
            errorString = "Wrong encoding type- must be RGB24!";
            return false;
        }

        if(patternWriters.front().getFrameCount() > 255) {
            errorString = "Pattern too long, must be < 256 frames";
            return false;
        }

        // Create the data structure to write to the device memory
        data.append((char)0x13);    // header
        data.append((char)0x37);
        data.append((char)patternWriters.front().getFrameCount());  // frame count
        data += patternWriters.front().getDataAsBinary();       // image data (RGB24, uncompressed)
    } else {
        // Create the data structure to write to the device memory
        // Animation table

        QByteArray patternData;

        data.append((char)0x31);    // header
        data.append((char)0x23);
        data.append((char)patternWriters.size()); // Number of patterns in the table
        data.append((char)PIXEL_COUNT);     // Number of LEDs in the pattern

        for (PatternWriter pattern : patternWriters) {
            // Make sure we have an image compatible with the BlinkyPendant
            if (pattern.getLedCount() != 10) {
                errorString = "Wrong pattern size- must be 10 pixels high!";
                return false;
            }
            if (pattern.getEncoding() != PatternWriter::RGB24) {
                errorString = "Wrong encoding type- must be RGB24!";
                return false;
            }

            if(pattern.getFrameCount() > 65535) {
                errorString = "Pattern too long, must be < 65535 frames";
                return false;
            }

            // Animation entry
            data.append((char)0);             // Encoding type (1 byte) (RGB24, uncompressed) (TODO)
            data += uint32ToByteArray(patternData.length());        // Data offset (4 bytes)
            data += uint16ToByteArrayBig(pattern.getFrameCount());   // Frame count (2 bytes)
            data += uint16ToByteArrayBig(0);                          // Frame delay (2 bytes) TODO

            // Make sure we have an image compatible with the BlinkyPendant
            patternData += pattern.getDataAsBinary();       // image data (RGB24, uncompressed)
        }

        data += patternData;
    }

    // TODO: Check if the data can fit in the device memory

    // Set up the commandQueue using the serial descriptor, and close the tape connection
    controller.close();
    commandQueue.open(portInfo);

    setProgress(0);

    // Queue the following commands:
    // 1. start write
    commandQueue.enqueue(BlinkyPendantCommands::startWrite());

    // 2-n. write data (aligned to 1024-byte sectors, 64 bytes at a time)
    commandQueue.enqueue(BlinkyPendantCommands::writeFlash(data));

    // n+1 stop write
    commandQueue.enqueue(BlinkyPendantCommands::stopWrite());

    return true;
}

bool BlinkyPendantUploader::updateFirmware(BlinkyController &controller)
{
    Q_UNUSED(controller);

    errorString = "Firmware update not currently supported for BlinkyPendant!";
    return false;
}

bool BlinkyPendantUploader::restoreFirmware(qint64)
{
    errorString = "Firmware update not currently supported for BlinkyPendant!";
    return false;
}

QString BlinkyPendantUploader::getErrorString() const
{
    return errorString;
}

QList<PatternWriter::Encoding> BlinkyPendantUploader::getSupportedEncodings() const
{
    QList<PatternWriter::Encoding> encodings;
    encodings.append(PatternWriter::RGB24);
    return encodings;
}

void BlinkyPendantUploader::cancel()
{
    qDebug() << "Cancel signalled, but not supported";
}

void BlinkyPendantUploader::handleError(QString error)
{
    qCritical() << error;

    commandQueue.close();

    emit(finished(false));
}

void BlinkyPendantUploader::handleCommandFinished(QString command, QByteArray returnData)
{
    Q_UNUSED(command);
    Q_UNUSED(returnData);

    qDebug() << "Command finished:" << command;
    setProgress(progress + 1);

    // TODO: Let the receiver handle this instead.
    if (command == "stopWrite") {
        commandQueue.close();
        emit(finished(true));
    }
}

void BlinkyPendantUploader::setProgress(int newProgress)
{
    // TODO: Calculate max progress
    int maxProgress = 10;

    progress = newProgress;
    emit(progressChanged((progress*100)/maxProgress));
}
