#include "leoblinkyuploader.h"
#include "usbutils.h"
#include "blinkycontroller.h"
#include "leoblinkycommands.h"
#include "bytearrayhelpers.h"

#include <vector>

// TODO: push the image conversions into here so they are less awkward.
#define PIXEL_COUNT (7*2)

#define PAGE_SIZE_BYTES     256    // Size unit for writes to flash


LeoBlinkyUploader::LeoBlinkyUploader(QObject *parent) :
    BlinkyUploader(parent)
{
    connect(&commandQueue, &SerialCommandQueue::errorOccured,
            this, &LeoBlinkyUploader::handleError);
    connect(&commandQueue, &SerialCommandQueue::commandFinished,
            this, &LeoBlinkyUploader::handleCommandFinished);
}

bool LeoBlinkyUploader::storePatterns(BlinkyController &controller,
                                        QList<PatternWriter> &patternWriters)
{
    // Probe for the blinkypendant version
    // TODO: Update the firmware first!
    QSerialPortInfo portInfo;
    if (!controller.getPortInfo(portInfo)) {
        errorString = "Couln't get port information!";
        return false;
    }


    /// Convert the patterns to page data
    QList<QByteArray> patternDatas;

    foreach (PatternWriter pattern, patternWriters) {
        QByteArray patternData;

        // Make sure we have an image compatible with the BlinkyPendant
        if (pattern.getLedCount() != PIXEL_COUNT) {
            errorString = "Wrong pattern size- must be n pixels!";
            return false;
        }
        if (pattern.getEncoding() != PatternWriter::RGB24) {
            errorString = "Wrong encoding type- must be RGB24!";
            return false;
        }

        // TODO: Max frame count?
//        if(pattern.getFrameCount() > 65535) {
//            errorString = "Pattern too long, must be < 65535 frames";
//            return false;
//        }

        // TODO: Duplicated in LeoBlinky::SendUpdate()
        QByteArray ledData = pattern.getDataAsBinary();
        for(int i = 0; i < ledData.length()/3; i++) {
            // ITU BT.709 RGB to luma conversion:
            int val = (0.2126*uint8_t(ledData[i*3])
                    + 0.7152*uint8_t(ledData[i*3+1])
                    + 0.0722*uint8_t(ledData[i*3+2]));

            patternData.append(char(val));
        }

        // Pad to a page boundary
        while(patternData.length() %PAGE_SIZE_BYTES != 0) {
            patternData.append(char(0xff));
        }

        patternDatas.append(patternData);

        qDebug() << "Pattern frame count: " << pattern.getFrameCount()
                 << " ledData length" << ledData.length()
                 << " patternData length" << patternData.length()
                 << " patternDatas length" << patternDatas.length();
    }

    /// Build the animation table
    QByteArray header;

    header += ByteArrayHelpers::uint32ToByteArrayLittle(0x12345678);  // header (4 bytes)
    header += ByteArrayHelpers::uint32ToByteArrayLittle(patternWriters.size()); // Animation count (4 bytes)


    // Then add an entry for each animation
    int animationAddress = PAGE_SIZE_BYTES;   // Animation data starts on page 1
    for(int i = 0; i < patternWriters.length(); i++) {
        header += ByteArrayHelpers::uint16ToByteArrayLittle(patternWriters[i].getFrameCount());
        header += ByteArrayHelpers::uint16ToByteArrayLittle(patternWriters[i].getLedCount());
        header += ByteArrayHelpers::uint16ToByteArrayLittle(patternWriters[i].getFrameDelay()); // TODO: Speed or delay?
        header += ByteArrayHelpers::uint32ToByteArrayLittle(animationAddress);
        animationAddress += patternDatas[i].length();
    }

    // Pad to a page boundary
    while(header.length() %PAGE_SIZE_BYTES != 0) {
        header.append(char(0xff));
    }

    qDebug() << "magic:"
            << " "  << (uint8_t)(header[0])
            << " " << (uint8_t)(header[1])
            << " " << (uint8_t)(header[2])
            << " " << (uint8_t)(header[3])
            << "pattern count: " << (uint8_t)(header[4])
            << " " << (uint8_t)(header[5])
            << " " << (uint8_t)(header[6])
            << " " << (uint8_t)(header[7])
            << "pattern0 framecount: " << (uint8_t)(header[8])
            << " " << (uint8_t)(header[9])
            << " " << (uint8_t)(header[10])
            << " " << (uint8_t)(header[11])
            << "pattern0 framedelay: " << (uint8_t)(header[12])
            << " " << (uint8_t)(header[13])
            << " " << (uint8_t)(header[14])
            << " " << (uint8_t)(header[15])
            << "pattern0 address: " << (uint8_t)(header[16])
            << " " << (uint8_t)(header[17])
            << " " << (uint8_t)(header[18])
            << " " << (uint8_t)(header[19]);

    qDebug() << "Animation count: " << patternWriters.length()
             << " header length" << header.length();


    QByteArray data;
    data += header;

    for(QByteArray patternData : patternDatas) {
        data += patternData;
    }

    // TODO: Check if the data can fit in the device memory

    // Set up the commandQueue using the serial descriptor, and close the tape connection
    controller.close();
    commandQueue.open(portInfo);

    setProgress(0);

    // Queue the following commands:
    // 1. Erase flash
    commandQueue.enqueue(LeoBlinkyCommands::eraseFlash());

    // 2-n. write data (aligned to 256-byte sectors)
    commandQueue.enqueue(LeoBlinkyCommands::writeData(data));

    // n. Reset device
    commandQueue.enqueue(LeoBlinkyCommands::reloadAnimations());

    maxProgress = commandQueue.length();

    return true;
}

QString LeoBlinkyUploader::getErrorString() const
{
    return errorString;
}

QList<PatternWriter::Encoding> LeoBlinkyUploader::getSupportedEncodings() const
{
    QList<PatternWriter::Encoding> encodings;
    encodings.append(PatternWriter::RGB24);
    return encodings;
}

void LeoBlinkyUploader::cancel()
{
    qDebug() << "Cancel signalled, but not supported";
}

void LeoBlinkyUploader::handleError(QString error)
{
    qCritical() << error;

    commandQueue.close();

    emit(finished(false));
}

void LeoBlinkyUploader::handleCommandFinished(QString command, QByteArray returnData)
{
    Q_UNUSED(command);
    Q_UNUSED(returnData);

    qDebug() << "Command finished:" << command;
    setProgress(progress + 1);

    // TODO: Let the receiver handle this instead.
    if (command == "reloadAnimations") {
        commandQueue.close();
        emit(finished(true));
    }
}

void LeoBlinkyUploader::setProgress(int newProgress)
{
    progress = newProgress;

    // Clip the progress so that it never reaches 100%.
    // It will be closed by the finished() signal.
    if (progress >= maxProgress)
        maxProgress = progress + 1;

    int progressPercent = (progress*100)/maxProgress;

    emit(progressChanged(progressPercent));
}
