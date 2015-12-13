#include "lightbuddyuploader.h"

#include "blinkycontroller.h"

#define FLASH_PAGE_SIZE 256

LightBuddyUploader::LightBuddyUploader(QObject *parent) :
    BlinkyUploader(parent)
{
    connect(&serialQueue,SIGNAL(error(QString)),
            this,SLOT(handleProgrammerError(QString)));
    connect(&serialQueue,SIGNAL(commandFinished(QString,QByteArray)),
            this,SLOT(handleProgrammerCommandFinished(QString,QByteArray)));
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

bool LightBuddyUploader::startUpload(BlinkyController& controller, std::vector<PatternWriter> patternWriters)
{
#define LIGHTBUDDY_UPLOADER

#if !defined(LIGHTBUDDY_UPLOADER)

    Q_UNUSED(controller);
    Q_UNUSED(patterns);

    // TODO: Support firmware upload for the lightbuddy
    errorString = "Animation upload not currently supported for Lightbuddy!";
    return false;

#else

    // 1. Make the patterns into data and store them in a vector
    // 2. Check that they will fit in the BlinkyTile memory (TODO!!!)
    // 3. Kick off the uploader state machine

    // For each pattern, append the image data to the sketch
    for (std::vector<PatternWriter>::iterator patternWriter = patternWriters.begin();
        patternWriter != patternWriters.end();
        ++patternWriter) {

        if(patternWriter->getEncoding() != PatternWriter::RGB24) {
            errorString = "Lightbuddy only supports RGB24 encoding";
            return false;
        }

        QByteArray data;
        // Build the header
        data.append((char)((patternWriter->getLedCount() >> 24) & 0xFF));
        data.append((char)((patternWriter->getLedCount() >> 16) & 0xFF));
        data.append((char)((patternWriter->getLedCount() >>  8) & 0xFF));
        data.append((char)((patternWriter->getLedCount()      ) & 0xFF));
        data.append((char)((patternWriter->getFrameCount() >> 24) & 0xFF));
        data.append((char)((patternWriter->getFrameCount() >> 16) & 0xFF));
        data.append((char)((patternWriter->getFrameCount() >>  8) & 0xFF));
        data.append((char)((patternWriter->getFrameCount()      ) & 0xFF));
        data.append((char)((patternWriter->getFrameDelay() >> 24) & 0xFF));
        data.append((char)((patternWriter->getFrameDelay() >> 16) & 0xFF));
        data.append((char)((patternWriter->getFrameDelay() >>  8) & 0xFF));
        data.append((char)((patternWriter->getFrameDelay()      ) & 0xFF));
        data.append((char)((patternWriter->getEncoding() >> 24) & 0xFF));
        data.append((char)((patternWriter->getEncoding() >> 16) & 0xFF));
        data.append((char)((patternWriter->getEncoding() >>  8) & 0xFF));
        data.append((char)((patternWriter->getEncoding()      ) & 0xFF));

        data += patternWriter->getData();

        while(data.count()%256 != 0)
            data.append((char)0x255);

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

    serialQueue.open(info);
    state = State_EraseFlash;
 //   state = State_FileNew;
    doWork();

    return true;
#endif // LIGHTBUDDY_UPLOADER
}

void LightBuddyUploader::doWork() {

    // 1. Erase the flash (slow! TODO: new firmware that's faster, or walk through the files to delete them)
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
        serialQueue.eraseFlash();
        //serialCommands.largestFile();
            state = State_FileNew;
        }
        break;
    case State_FileNew:
        {
            serialQueue.fileNew(flashData.front().size());
            //serialCommands.fileNew(256);
            state = State_WriteFileData;
        }
        break;
    case State_WriteFileData:
        {
            for(int offset = 0; offset < flashData.front().size(); offset += FLASH_PAGE_SIZE) {
                serialQueue.writePage(sector, offset, flashData.front().mid(offset, FLASH_PAGE_SIZE));
            }
            flashData.pop_front();

            serialQueue.reloadAnimations();

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

    if(serialQueue.isConnected()) {
        serialQueue.close();
    }

    emit(finished(false));
}

void LightBuddyUploader::handleProgrammerCommandFinished(QString command, QByteArray returnData) {
    Q_UNUSED(returnData);

    qDebug() << "Command finished:" << command;
//    setProgress(progress + 1);

    // TODO: This is poor logic.

    if(command == "eraseFlash")
        doWork();

    // TODO: Just for testing
    if(command == "largestFile")
        doWork();

    if(command == "fileNew") {
        // record sector for new file here.
        for(int i = 0; i < returnData.count();i++)
            qDebug() << i << ": " << (int)returnData.at(i)
                     << "(" << returnData.at(i) << ")";

        sector = ((int)returnData.at(2) << 24)
                + ((int)returnData.at(3) << 16)
                + ((int)returnData.at(4) << 8)
                + ((int)returnData.at(5) << 0);
        qDebug() << "sector: " << sector;

        doWork();
    }

    // If it was a reload animation command, we might have more work, or might be done.
    if(command == "reloadAnimations") {
        if(state == State_Done) {
            serialQueue.close();
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
