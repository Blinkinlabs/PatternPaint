#include "lightbuddyuploader.h"

#include "blinkycontroller.h"
#include "lightbuddycommands.h"

#include "bytearrayhelpers.h"

#define FLASH_PAGE_SIZE 256
#define MAX_PATTERN_SIZE 507648

LightBuddyUploader::LightBuddyUploader(QObject *parent) :
    BlinkyUploader(parent)
{
    connect(&commandQueue, &SerialCommandQueue::errorOccured,
            this, &LightBuddyUploader::handleError);
    connect(&commandQueue, &SerialCommandQueue::commandFinished,
            this, &LightBuddyUploader::handleCommandFinished);
}

QList<PatternWriter::Encoding> LightBuddyUploader::getSupportedEncodings() const
{
    QList<PatternWriter::Encoding> encodings;
    encodings.append(PatternWriter::RGB24);
    return encodings;
}

void LightBuddyUploader::cancel()
{
    qDebug() << "Cancel signalled, but not supported";
}

bool LightBuddyUploader::storePatterns(BlinkyController &controller,
                                     QList<PatternWriter> &patternWriters)
{
    // 1. Make the patterns into data and store them in a vector
    // 2. Check that they will fit in the BlinkyTile memory
    // 3. Kick off the uploader state machine

    maxProgress = 1;    // For the initial erase command

    // For each pattern, append the image data to the sketch
    for (PatternWriter patternWriter : patternWriters) {
        if (patternWriter.getEncoding() != PatternWriter::RGB24) {
            errorString = "Lightbuddy only supports RGB24 encoding";
            return false;
        }

        if (patternWriter.getDataAsBinary().count() > MAX_PATTERN_SIZE) {
            errorString = QString("Pattern too big to fit in memory! Size=%1, Max size=%2").arg(
                patternWriter.getDataAsBinary().count()).arg(MAX_PATTERN_SIZE);
            return false;
        }

        // Workaround for color order swap on lightbuddy- change RGB to BGR.
        // TODO: Update the lightbuddy firmware so we don't need to do this.
        qDebug() << "size: " << patternWriter.getDataAsBinary().count()/3;
        QByteArray mungedPatternData;
        for(int pixel = 0; pixel < patternWriter.getDataAsBinary().count()/3; pixel++) {
            mungedPatternData.append(patternWriter.getDataAsBinary().at(pixel*3+2));
            mungedPatternData.append(patternWriter.getDataAsBinary().at(pixel*3+1));
            mungedPatternData.append(patternWriter.getDataAsBinary().at(pixel*3+0));
        }

        QByteArray data;

        // Build the header
        data += ByteArrayHelpers::uint32ToByteArrayBig(patternWriter.getLedCount());
        data += ByteArrayHelpers::uint32ToByteArrayBig(patternWriter.getFrameCount());
        data += ByteArrayHelpers::uint32ToByteArrayBig(patternWriter.getFrameDelay());
        data += ByteArrayHelpers::uint32ToByteArrayBig(patternWriter.getEncoding());

        //data += patternWriter.getData();
        data += mungedPatternData;

        while (data.count()%FLASH_PAGE_SIZE != 0)
            data.append((char)0x255);

        flashData.append(data);

        // Calculate the number of serial transactions that will occur in this upload
        maxProgress += data.count()/FLASH_PAGE_SIZE+2;
    }

    setProgress(0);

    QSerialPortInfo info;
    controller.getPortInfo(info);

    if (controller.isConnected())
        controller.close();

    commandQueue.open(info);
    state = State_EraseFlash;
    doWork();

    return true;
}

void LightBuddyUploader::doWork()
{
    // 1. Erase the flash (slow! TODO: new firmware that's faster, or walk through the files to delete them)
    // 2. For each pattern:
    // a. create a new file
    // b. if creation successful, upload image data
    // 3. Reset controller

    qDebug() << "In doWork state=" << state;

    // Continue the current state
    switch (state) {
    // TODO: Test that the patterns will fit before starting!

    case State_EraseFlash:
    {
        commandQueue.enqueue(LightBuddyCommands::eraseFlash());

        state = State_FileNew;
        break;
    }
    case State_FileNew:
    {
        commandQueue.enqueue(LightBuddyCommands::fileNew(flashData.front().size()));
        // serialCommands.fileNew(256);
        state = State_WriteFileData;
        break;
    }
    case State_WriteFileData:
    {
        for (int offset = 0; offset < flashData.front().size(); offset += FLASH_PAGE_SIZE)
            commandQueue.enqueue(LightBuddyCommands::writePage(sector, offset,
                                                               flashData.front().mid(offset,
                                                                                     FLASH_PAGE_SIZE)));

        flashData.pop_front();

        commandQueue.enqueue(LightBuddyCommands::reloadAnimations());

        if (flashData.size() > 0)
            state = State_FileNew;
        else
            state = State_Done;
        break;
    }
    case State_Done:
        break;
    }
}

QString LightBuddyUploader::getErrorString() const
{
    return errorString;
}

void LightBuddyUploader::handleError(QString error)
{
    errorString = error;
    qCritical() << error;

    commandQueue.close();

    emit(finished(false));
}

void LightBuddyUploader::handleCommandFinished(QString command, QByteArray returnData)
{
    Q_UNUSED(returnData);

    setProgress(progress + 1);

    if (command == "eraseFlash")
        doWork();

    if (command == "fileNew") {
        sector = ByteArrayHelpers::byteArrayToUint32Big(returnData.mid(2, 4));
        qDebug() << "sector: " << sector;

        doWork();
    }

    // If it was a reload animation command, we might have more work, or might be done.
    if (command == "reloadAnimations") {
        if (state == State_Done) {
            // TODO: Separate these with a small delay?
            commandQueue.close();
            emit(finished(true));
        } else {
            doWork();
        }
    }
}

void LightBuddyUploader::setProgress(int newProgress)
{
    progress = newProgress;

    // Clip the progress so that it never reaches 100%.
    // It will be closed by the finished() signal.
    if (progress >= maxProgress)
        maxProgress = progress + 1;

    int progressPercent = (progress*100)/maxProgress;

    emit(progressChanged(progressPercent));
}
