#include "eightbyeightuploader.h"

#include "blinkycontroller.h"
#include "eightbyeightcommands.h"

#include "bytearrayhelpers.h"


// TODO: Duplicated in eightbyeightcommands.cpp
#define CHUNK_SIZE_BYTES 64 // TODO: Small so we don't overflow the usb/serial converter?

// TODO: This isn't exact, depends on filesystem
#define FLASH_MEMORY_AVAILABLE (1048576) // 1MB

EightByEightUploader::EightByEightUploader(QObject *parent) :
    BlinkyUploader(parent)
{
    connect(&commandQueue, &SerialCommandQueue::errorOccured,
            this, &EightByEightUploader::handleError);
    connect(&commandQueue, &SerialCommandQueue::commandStillRunning,
            this, &EightByEightUploader::handleCommandStillRunning);
    connect(&commandQueue, &SerialCommandQueue::commandFinished,
            this, &EightByEightUploader::handleCommandFinished);
    connect(&commandQueue, &SerialCommandQueue::lastCommandFinished,
            this, &EightByEightUploader::handleLastCommandFinished);
}

QList<PatternWriter::Encoding> EightByEightUploader::getSupportedEncodings() const
{
    QList<PatternWriter::Encoding> encodings;
    encodings.append(PatternWriter::RGB24);
    return encodings;
}

void EightByEightUploader::cancel()
{
    qDebug() << "Cancel signalled, but not supported";
}

bool EightByEightUploader::storePatterns(BlinkyController &controller,
                                     QList<PatternWriter> &patternWriters)
{
    // 1. Make the patterns into data and store them in a vector
    // 2. TODO: Check that they will fit in the EightByEight memory
    // 3. Kick off the uploader state machine

    maxProgress = 1;    // For the initial erase command

    // For each pattern, append the image data to the sketch
    for (PatternWriter patternWriter : patternWriters) {
        if (patternWriter.getEncoding() != PatternWriter::RGB24) {
            errorString = "EightByEight only supports RGB24 encoding";
            return false;
        }

        QByteArray data;

        // Pattern file format (version 0):
        // Offset 0-2: char[3] 'PAT' (magic header)
        // Offset 3: uint32_t fileVersion (1)
        // Offset 7: uint32_t encoding
        // Offset 11: uint32_t ledCount (leds)
        // Offset 15: uint32_t frameCount (frames)
        // Offset 19: uint32_t frameDelay (ms)
        // Offset 23: uint32_t frameDataSize (bytes)
        // Offset 27+: frame data

        data += "PAT";
        data += ByteArrayHelpers::uint32ToByteArrayBig(1); // version
        data += ByteArrayHelpers::uint32ToByteArrayBig(patternWriter.getEncoding());
        data += ByteArrayHelpers::uint32ToByteArrayBig(patternWriter.getLedCount());
        data += ByteArrayHelpers::uint32ToByteArrayBig(patternWriter.getFrameCount());
        data += ByteArrayHelpers::uint32ToByteArrayBig(patternWriter.getFrameDelay());
        data += ByteArrayHelpers::uint32ToByteArrayBig(patternWriter.getDataAsBinary().length());
        data += patternWriter.getDataAsBinary();

        qDebug() << "pattern length:" << data.length();

        flashData.append(data);

        // Calculate the number of serial transactions that will occur in this upload
        // TODO
        maxProgress += 4 + 2*data.count()/CHUNK_SIZE_BYTES;
    }

    setProgress(0);
    setDialogText();

    QSerialPortInfo info;
    controller.getPortInfo(info);

    if (controller.isConnected())
        controller.close();

    commandQueue.open(info);
    state = State_checkFirmwareVersion;
    doWork();

    return true;
}

void EightByEightUploader::doWork()
{
    // 1. Lock file access on the EightByEight, to prevent it from interfering with the upload
    // 2. Erase the flash (slow! TODO: new firmware that's faster, or walk through the files to delete them)
    // 3. For each pattern:
    // a. create a new file
    // b. load data into the file
    // c. close the file
    // d. open the file in read mode
    // e. read the file back for verification
    // f. close the file
    // 4. Unlock file access of the EightByEight, to allow it to reload the patterns and start playback

    // Continue the current state
    switch (state) {
    // TODO: Test that the patterns will fit before starting!
    case State_checkFirmwareVersion:
    {
        commandQueue.enqueue(EightByEightCommands::getFirmwareVersion());
        break;
    }

    case State_lockFileAccess:
    {
        commandQueue.enqueue(EightByEightCommands::lockFileAccess());
        break;
    }

    case State_erasePatterns:
    {
        // TODO: new command to just delete the patterns...
        commandQueue.enqueue(EightByEightCommands::formatFilesystem());

        break;
    }
    case State_WriteFile:
    {
        QString fileName = QString("/p/%1").arg(patternIndex);

        qDebug() << "Writing pattern:" << patternIndex
                 << "fileName:" << fileName;

        commandQueue.enqueue(EightByEightCommands::openFile(fileName,
                                                            EightByEightCommands::FileMode_Write));
        commandQueue.enqueue(EightByEightCommands::write(flashData.front()));
        commandQueue.enqueue(EightByEightCommands::closeFile());

        commandQueue.enqueue(EightByEightCommands::openFile(fileName,
                                                            EightByEightCommands::FileMode_Read));
        commandQueue.enqueue(EightByEightCommands::verify(flashData.front()));
        commandQueue.enqueue(EightByEightCommands::closeFile());
    }
        break;

    case State_unlockFileAccess:
    {
        commandQueue.enqueue(EightByEightCommands::unlockFileAccess());
        break;
    }

    case State_Done:
        break;
    }
}

QString EightByEightUploader::getErrorString() const
{
    return errorString;
}

void EightByEightUploader::handleError(QString error)
{
    qCritical() << error;
    errorString = error;

    commandQueue.close();

    emit(finished(false));
}

void EightByEightUploader::handleCommandStillRunning(QString command)
{
    Q_UNUSED(command);

    maxProgress++;
    setProgress(progress + 1);
}

void EightByEightUploader::handleCommandFinished(QString command, QByteArray returnData)
{
    Q_UNUSED(command);
    Q_UNUSED(returnData);

    setProgress(progress + 1);
}

void EightByEightUploader::handleLastCommandFinished()
{
    // TODO: Moveme to doWork() ?
    switch (state) {
    case State_checkFirmwareVersion:
        // Check version?

        state = State_lockFileAccess;
        doWork();
        break;

    case State_lockFileAccess:
        state = State_erasePatterns;
        doWork();
        break;

    case State_erasePatterns:
        state = State_WriteFile;
        patternIndex = 0;
        doWork();
        break;

    case State_WriteFile:
        if (flashData.size() > 0) {
            flashData.pop_front();
            patternIndex++;
        }

        // If there are patterns remaining, send the next one
        if (flashData.size() > 0) {
            state = State_WriteFile;
            doWork();
        }

        // Otherwise transition to finished
        else {
            state = State_unlockFileAccess;
            doWork();
        }
        break;

    case State_unlockFileAccess:
        commandQueue.close();
        state = State_Done;
        emit(finished(true));
        break;

    default:
        qCritical() << "Got a last command finished signal when not expected";
        break;
    }
}

void EightByEightUploader::setProgress(int newProgress)
{
    progress = newProgress;

    // Clip the progress so that it never reaches 100%.
    // It will be closed by the finished() signal.
    if (progress >= maxProgress)
        maxProgress = progress + 1;

    int progressPercent = (progress*100)/maxProgress;

    emit(progressChanged(progressPercent));
}

void EightByEightUploader::setDialogText()
{
    int flashUsed = 0;

    QString textLabel;
    textLabel.append("Saving to Blinky...\n");
    textLabel.append("\n");

    for (QByteArray& section : flashData)
        flashUsed += section.length();

    float flashUsedPercent = float(flashUsed)*100/FLASH_MEMORY_AVAILABLE;
    textLabel.append(QString("Flash used: %1%").arg(QString::number(flashUsedPercent,'f', 1)));

    emit(setText(textLabel));
}
