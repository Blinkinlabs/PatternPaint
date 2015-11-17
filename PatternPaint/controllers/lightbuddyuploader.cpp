#include "lightbuddyuploader.h"

#define FLASH_PAGE_SIZE 256

LightBuddyUploader::LightBuddyUploader(QObject *parent) :
    PatternUploader(parent)
{
    connect(&programmer,SIGNAL(error(QString)),
            this,SLOT(handleProgrammerError(QString)));
    connect(&programmer,SIGNAL(commandFinished(QString,QByteArray)),
            this,SLOT(handleProgrammerCommandFinished(QString,QByteArray)));
}


QList<PatternWriter::Encoding> LightBuddyUploader::getSupportedEncodings() const
{
    QList<PatternWriter::Encoding> encodings;
    encodings.append(PatternWriter::RGB24);
    return encodings;
}

bool LightBuddyUploader::startUpload(BlinkyController& controller, std::vector<PatternWriter> patterns)
{
    Q_UNUSED(controller);
    Q_UNUSED(patterns);

    // TODO: Support firmware upload for the lightbuddy
    errorString = "Animation upload not currently supported for Lightbuddy!";
    return false;


#if defined(LIGHTBUDDY_UPLOADER)
    // 1. Make the patterns into data and store them in a vector
    // 2. Check that they will fit in the BlinkyTile memory (TODO!!!)
    // 3. Kick off the uploader state machine

    // For each pattern, append the image data to the sketch
    for (std::vector<Pattern>::iterator pattern = patterns.begin();
        pattern != patterns.end();
        ++pattern) {

        const int animationType = 0;

        QByteArray data;
        // Build the header
        data.append((char)((pattern->ledCount >> 24) & 0xFF));
        data.append((char)((pattern->ledCount >> 16) & 0xFF));
        data.append((char)((pattern->ledCount >>  8) & 0xFF));
        data.append((char)((pattern->ledCount      ) & 0xFF));
        data.append((char)((pattern->frameCount >> 24) & 0xFF));
        data.append((char)((pattern->frameCount >> 16) & 0xFF));
        data.append((char)((pattern->frameCount >>  8) & 0xFF));
        data.append((char)((pattern->frameCount      ) & 0xFF));
        data.append((char)((pattern->frameDelay >> 24) & 0xFF));
        data.append((char)((pattern->frameDelay >> 16) & 0xFF));
        data.append((char)((pattern->frameDelay >>  8) & 0xFF));
        data.append((char)((pattern->frameDelay      ) & 0xFF));
        data.append((char)((animationType >> 24) & 0xFF));
        data.append((char)((animationType >> 16) & 0xFF));
        data.append((char)((animationType >>  8) & 0xFF));
        data.append((char)((animationType      ) & 0xFF));

        // Recompress the pattern in RGB24
        Pattern recompressedPattern(pattern->image, 0, Pattern::RGB24, Pattern::RGB);
        data += recompressedPattern.data;

        flashData.append(data);
    }


    setProgress(0);
    // TODO: Calculate this based on feedback from the programmer.
    setMaxProgress(300);

    QSerialPortInfo info;
    controller.getPortInfo(info);

    if(controller.isConnected()) {
        controller.close();
    }

    programmer.open(info);
    state = State_EraseFlash;
    doWork();

    return true;
#endif // LIGHTBUDDY_UPLOADER
}

void LightBuddyUploader::doWork() {

    // 1. Erase the flash (slow! TODO: new firmware that's faster)
    // 2. For each pattern:
    //    a. create a new file
    //    b. if creation successful, upload image data
    // 3. Reset controller

    qDebug() << "In doWork state=" << state;

    // Continue the current state
    switch(state) {

    // TODO: Test that the patterns will fit before starting!

    case State_EraseFlash:
        {
            programmer.eraseFlash();
            state = State_FileNew;
        }
        break;
    case State_FileNew:
        {
            programmer.fileNew(flashData.front().size());
            state = State_WriteFileData;
        }
        break;
    case State_WriteFileData:
        {
            // TODO: Extract this from the fileNew() call response
            int sector = 0;

            for(int offset = 0; offset < flashData.front().size(); offset += FLASH_PAGE_SIZE) {
                programmer.writePage(sector, offset, flashData.front().mid(offset, FLASH_PAGE_SIZE));
            }
            flashData.pop_front();

            programmer.reloadAnimations();

            if(flashData.size() > 0) {
                state = State_FileNew;
            }
            else {
                state = State_Done;
            }
        }
        break;
    case State_Done:
        break;
    }
}


bool LightBuddyUploader::upgradeFirmware(BlinkyController& controller)
{
    Q_UNUSED(controller);

    // TODO: Support firmware upload for the lightbuddy
    errorString = "Firmware update not currently supported for Lightbuddy!";
    return false;
}

bool LightBuddyUploader::upgradeFirmware(int)
{
    // TODO: Support firmware upload for the lightbuddy
    errorString = "Firmware update not currently supported for Lightbuddy!";
    return false;
}

QString LightBuddyUploader::getErrorString() const
{
    return errorString;
}

void LightBuddyUploader::handleProgrammerError(QString error) {
    qCritical() << error;

    if(programmer.isConnected()) {
        programmer.close();
    }

    emit(finished(false));
}

void LightBuddyUploader::handleProgrammerCommandFinished(QString command, QByteArray returnData) {
    Q_UNUSED(returnData);

    qDebug() << "Command finished:" << command;
//    setProgress(progress + 1);

    // TODO: This is poor logic.

    // If it was a file new command, we have more work to do
    if(command == "fileNew") {
        doWork();
    }

    // If it was a reload animation command, we might have more work, or might be done.
    if(command == "reloadAnimations") {
        if(state == State_Done) {
            programmer.close();
            emit(finished(true));
        }
        else {
            doWork();
        }
    }
}

void LightBuddyUploader::setProgress(int newProgress) {
//    progress = newProgress;
    emit(progressChanged(newProgress));
}

void LightBuddyUploader::setMaxProgress(int newMaxProgress) {
    emit(maxProgressChanged(newMaxProgress));
}
