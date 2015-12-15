#include "blinkypendantuploader.h"

#include "usbutils.h"
#include "blinkycontroller.h"
#include "blinkypendantcommands.h"

#define BLINKY_PENDANT_VERSION_1 256
#define BLINKY_PENDANT_VERSION_2 512

namespace {

// TODO: Dupe in lightbuddycommands.cpp
QByteArray encodeInt(int data)
{
    QByteArray output;
    output.append((char)((data >> 24) & 0xFF));
    output.append((char)((data >> 16) & 0xFF));
    output.append((char)((data >>  8) & 0xFF));
    output.append((char)((data) & 0xFF));
    return output;
}

QByteArray encodeWord(int data)
{
    QByteArray output;
    output.append((char)((data >>  8) & 0xFF));
    output.append((char)((data) & 0xFF));
    return output;
}

}

BlinkyPendantUploader::BlinkyPendantUploader(QObject *parent) :
    BlinkyUploader(parent)
{
    connect(&commandQueue, SIGNAL(error(QString)),
            this, SLOT(handlecommandQueueError(QString)));
    connect(&commandQueue, SIGNAL(commandFinished(QString, QByteArray)),
            this, SLOT(handlecommandQueueCommandFinished(QString, QByteArray)));
}

bool BlinkyPendantUploader::startUpload(BlinkyController &controller,
                                        std::vector<PatternWriter> patterns)
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
        if (patterns.at(0).getLedCount() != 10) {
            errorString = "Wrong pattern size- must be 10 pixels high!";
            return false;
        }
        if (patterns.at(0).getEncoding() != PatternWriter::RGB24) {
            errorString = "Wrong encoding type- must be RGB24!";
            return false;
        }

        if(patterns.at(0).getFrameCount() > 255) {
            errorString = "Pattern too long, must be < 256 frames";
            return false;
        }

        // Create the data structure to write to the device memory
        data.append((char)0x13);    // header
        data.append((char)0x37);
        data.append((char)patterns.at(0).getFrameCount());  // frame count
        data += patterns.at(0).getData();       // image data (RGB24, uncompressed)
    } else {
        // Create the data structure to write to the device memory
        // Animation table

        QByteArray patternData;

        data.append((char)0x31);    // header
        data.append((char)0x23);
        data.append((char)patterns.size()); // Number of patterns in the table
        data.append((char)PIXEL_COUNT);     // Number of LEDs in the pattern

        for (std::vector<PatternWriter>::iterator pattern = patterns.begin();
             pattern != patterns.end();
             ++pattern) {
            // Make sure we have an image compatible with the BlinkyPendant
            if (pattern->getLedCount() != 10) {
                errorString = "Wrong pattern size- must be 10 pixels high!";
                return false;
            }
            if (pattern->getEncoding() != PatternWriter::RGB24) {
                errorString = "Wrong encoding type- must be RGB24!";
                return false;
            }

            if(pattern->getFrameCount() > 65535) {
                errorString = "Pattern too long, must be < 65535 frames";
                return false;
            }

            // Animation entry
            data.append((char)0);             // Encoding type (1 byte) (RGB24, uncompressed) (TODO)
            data += encodeInt(patternData.length());        // Data offset (4 bytes)
            data += encodeWord(pattern->getFrameCount());   // Frame count (2 bytes)
            data += encodeWord(0);                          // Frame delay (2 bytes) TODO

            // Make sure we have an image compatible with the BlinkyPendant
            patternData += pattern->getData();       // image data (RGB24, uncompressed)
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
    commandQueue.enqueue(BlinkyPendantCommands::writeData(data));

    // n+1 stop write
    commandQueue.enqueue(BlinkyPendantCommands::stopWrite());

    return true;
}

bool BlinkyPendantUploader::upgradeFirmware(BlinkyController &controller)
{
    Q_UNUSED(controller);

    errorString = "Firmware update not currently supported for BlinkyPendant!";
    return false;
}

bool BlinkyPendantUploader::upgradeFirmware(int)
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
    progress = newProgress;
    emit(progressChanged(static_cast<float>(progress)/10));
}
