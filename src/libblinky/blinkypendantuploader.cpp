#include "blinkypendantuploader.h"

#include "usbutils.h"
#include "blinkycontroller.h"
#include "blinkypendantcommands.h"
#include "blinkypendantuploaddata.h"

#include <QSettings>

#define BLINKY_PENDANT_VERSION_1 0x0100
#define BLINKY_PENDANT_VERSION_2 0x0200
#define BLINKY_PENDANT_VERSION_3 0x0300

#define BLINKYPENDANT_DISPLAYMODE_DEFAULT "POV"


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
    QByteArray data;

    // Probe for the blinkypendant version
    // TODO: Update the firmware first!
    QSerialPortInfo portInfo;
    if (!controller.getPortInfo(portInfo)) {
        errorString = "Couln't get port information!";
        return false;
    }

    int version = usbUtils::getVersionForDevice(portInfo.vendorIdentifier(),
                                                portInfo.productIdentifier());

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

    }
    else {
        // TODO: Pass this in somehow.
        QSettings settings;
        QString displayModeString = settings.value("BlinkyPendant/displayMode",
                                                   BLINKYPENDANT_DISPLAYMODE_DEFAULT).toString();
        BlinkyPendantUploadData::DisplayMode displayMode;
        if(displayModeString == "Timed") {
            displayMode = BlinkyPendantUploadData::DisplayMode::TIMED;
        }
        else {
            displayMode = BlinkyPendantUploadData::DisplayMode::POV;
        }

        BlinkyPendantUploadData uploadData;

        if (!uploadData.init(displayMode, patternWriters)) {
            errorString = uploadData.errorString;
            return false;
        }

        data = uploadData.data;
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

    // TODO: This might be partially depleted before this command is run?
    maxProgress = commandQueue.length();

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
    progress = newProgress;

    // Clip the progress so that it never reaches 100%.
    // It will be closed by the finished() signal.
    if (progress >= maxProgress)
        maxProgress = progress + 1;

    int progressPercent = (progress*100)/maxProgress;

    emit(progressChanged(progressPercent));
}
