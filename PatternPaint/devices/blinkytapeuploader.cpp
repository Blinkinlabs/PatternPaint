#include "blinkytapeuploader.h"
#include "avruploaddata.h"

#include "ProductionSketch.h"

#include <QDebug>

// TODO: Combine this with the definitions in avruploaddata.cpp
#define FLASH_MEMORY_AVAILABLE          0x7000  // Amount of application space in the flash

/// Interval between polling the serial port list for new devices
#define BOOTLOADER_POLL_INTERVAL 200

/// Maximum time to wait before giving up on finding a bootloader
#define BOOTLOADER_POLL_TIMEOUT 10000


/// How long to wait between receiving notification that the programmer has been
/// reset, and notifying the caller that we are finished
#define PROGRAMMER_RESET_DELAY 500

/// Length of character buffer for debug messages
#define BUFF_LENGTH 100

BlinkyTapeUploader::BlinkyTapeUploader(QObject *parent) :
    PatternUploader(parent)
{
    bootloaderResetTimer = new QTimer(this);

    state = State_Ready;

    connect(&programmer,SIGNAL(error(QString)),
            this,SLOT(handleProgrammerError(QString)));
    connect(&programmer,SIGNAL(commandFinished(QString,QByteArray)),
            this,SLOT(handleProgrammerCommandFinished(QString,QByteArray)));
}

void BlinkyTapeUploader::handleProgrammerError(QString error) {
    qCritical() << error;

    if(programmer.isConnected()) {
        programmer.close();
    }

    emit(finished(false));
}

void BlinkyTapeUploader::handleProgrammerCommandFinished(QString command, QByteArray returnData) {
    Q_UNUSED(returnData);

//    qDebug() << "Command finished:" << command;
    setProgress(progress + 1);

    // we know reset is the last command, so the BlinkyTape should be ready soon.
    // Schedule a timer to emit the message shortly.
    // TODO: Let the receiver handle this instead.
    if(command == "reset") {
        QTimer::singleShot(PROGRAMMER_RESET_DELAY, this,SLOT(handleResetTimer()));
    }
}

void BlinkyTapeUploader::handleResetTimer()
{
    emit(finished(true));
}

void BlinkyTapeUploader::setProgress(int newProgress) {
    progress = newProgress;
    emit(progressChanged(progress));
}

void BlinkyTapeUploader::setMaxProgress(int newMaxProgress) {
    emit(maxProgressChanged(newMaxProgress));
}

bool BlinkyTapeUploader::startUpload(BlinkyController& tape, std::vector<PatternWriter> patterns) {
    /// Create the compressed image and check if it will fit into the device memory
    avrUploadData data;
    if(!data.init(patterns)) {
        errorString = data.errorString;
        return false;
    }

    // TODO: Only write the sketch portion if we absolutely have to, to lower the risk
    // that we trash the firmware if there is a problem later

    // The entire sketch must fit into the available memory, minus a single page
    // at the end of flash for the configuration header
    // TODO: Verify this by looking at each section individually instead, making sure
    // none of them overlap or extend outside of available memory?
    if(data.sketch.length() + data.patternData.length() + data.patternTable.length() > FLASH_MEMORY_AVAILABLE) {
        qCritical() << "sketch can't fit into memory!";

        errorString = QString("Sorry! The Pattern is a bit too big to fit in BlinkyTape memory! Avaiable space=%1, Pattern size=%2")
                              .arg(FLASH_MEMORY_AVAILABLE)
                              .arg(data.sketch.length() + data.patternData.length() + data.patternTable.length());
        return false;
    }

    // Put the sketch, pattern, and metadata into the programming queue.
    qDebug() << "Sketch address: " << data.sketchAddress << ", size: " << data.sketch.length();
    qDebug() << "pattern data address: " << data.patternDataAddress << ", size: " << data.patternData.length();
    qDebug() << "pattern table address: " << data.patternTableAddress << ", size: " << data.patternTable.length();

    flashData.push_back(FlashSection(data.sketchAddress,        data.sketch));
    flashData.push_back(FlashSection(data.patternDataAddress,  data.patternData));
    flashData.push_back(FlashSection(data.patternTableAddress, data.patternTable));

    return startUpload(tape);
}


bool BlinkyTapeUploader::upgradeFirmware(BlinkyController& tape) {
    QByteArray sketch = QByteArray(reinterpret_cast<const char*>(PRODUCTION_DATA),PRODUCTION_LENGTH);

    char buff[BUFF_LENGTH];
    snprintf(buff, BUFF_LENGTH, "Sketch size: %iB",
             sketch.length()),
    qDebug() << buff;

    // The entire sketch must fit into the available memory, minus a single page
    // at the end of flash for the configuration header
    // TODO: Could save ~100 bytes if we let the sketch spill into the unused portion
    // of the header.
    if(sketch.length() > FLASH_MEMORY_AVAILABLE) {
        qDebug() << "sketch can't fit into memory!";

        errorString = QString("Sorry! The Pattern is a bit too big to fit in BlinkyTape memory right now. We're working on improving this! Avaiable space=%1, Pattern size=%2")
                .arg(FLASH_MEMORY_AVAILABLE)
                .arg(sketch.length());
        return false;
    }

    // Put the sketch, pattern, and metadata into the programming queue.
    flashData.push_back(FlashSection(PRODUCTION_ADDRESS, sketch));

    return startUpload(tape);
}

bool BlinkyTapeUploader::upgradeFirmware(int timeout) {
    QByteArray sketch = QByteArray(reinterpret_cast<const char*>(PRODUCTION_DATA),PRODUCTION_LENGTH);

    bootloaderPollTimeout = timeout;

    char buff[BUFF_LENGTH];
    snprintf(buff, BUFF_LENGTH, "Sketch size: %iB",
             sketch.length()),
    qDebug() << buff;

    // The entire sketch must fit into the available memory, minus a single page
    // at the end of flash for the configuration header
    // TODO: Could save ~100 bytes if we let the sketch spill into the unused portion
    // of the header.
    if(sketch.length() > FLASH_MEMORY_AVAILABLE) {
        qDebug() << "sketch can't fit into memory!";

        errorString = QString("Sorry! The Pattern is a bit too big to fit in BlinkyTape memory right now. We're working on improving this! Avaiable space=%1, Pattern size=%2")
                .arg(FLASH_MEMORY_AVAILABLE)
                .arg(sketch.length());
        return false;
    }

    // Put the sketch, pattern, and metadata into the programming queue.
    flashData.push_back(FlashSection(PRODUCTION_ADDRESS, sketch));

    setProgress(0);
    // TODO: Calculate this based on feedback from the programmer.
    setMaxProgress(300);

    stateStartTime = QDateTime::currentDateTime();
    state = State_WaitForBootloaderPort;
    bootloaderResetTimer->singleShot(0,this,SLOT(doWork()));
    return true;
}


bool BlinkyTapeUploader::startUpload(BlinkyController& tape) {
    setProgress(0);
    // TODO: Calculate this based on feedback from the programmer.
    setMaxProgress(300);

    bootloaderPollTimeout = BOOTLOADER_POLL_TIMEOUT;

    // Now, start the polling processes to detect a new bootloader
    // We can't reset if we weren't already connected...
    if(!tape.isConnected()) {
        errorString = "Not connected to a tape, cannot upload again";
        return false;
    }

    // Next, tell the tape to reset.
    tape.reset();

    stateStartTime = QDateTime::currentDateTime();
    state = State_WaitForBootloaderPort;
    bootloaderResetTimer->singleShot(0,this,SLOT(doWork()));
    return true;
}

QString BlinkyTapeUploader::getErrorString() const
{
    return errorString;
}

void BlinkyTapeUploader::doWork() {
    // TODO: This flow is really ungainly

    //qDebug() << "In doWork state=" << state;

    // Continue the current state
    switch(state) {
    case State_WaitForBootloaderPort:
        {
            // Scan to see if there is a bootloader present
            QList<QSerialPortInfo> postResetTapes
                    = BlinkyTape::probeBootloaders();

            // If we didn't detect a bootloader and still have time, then queue the timer and
            // wait. Otherwise, we timed out, so fail.
            if(postResetTapes.length() == 0) {
                if((bootloaderPollTimeout > 0)
                        && (stateStartTime.msecsTo(QDateTime::currentDateTime())
                        > bootloaderPollTimeout)) {
                    handleProgrammerError("Timeout waiting for a bootloader device");
                    return;
                }

                bootloaderResetTimer->singleShot(BOOTLOADER_POLL_INTERVAL,this,SLOT(doWork()));
                return;
            }

            qDebug() << "Bootloader waiting on: " << postResetTapes.at(0).portName();

            // Don't connect immediately, the device might need a short time to settle down
            bootloaderResetTimer->singleShot(PROGRAMMER_RESET_DELAY,this,SLOT(doWork()));
            state = State_WaitAfterBootloaderPort;
        }
        break;

    case State_WaitAfterBootloaderPort:
        {
            QList<QSerialPortInfo> postResetTapes
                    = BlinkyTape::probeBootloaders();

            // If we didn't detect a bootloader and still have time, then queue the timer and
            // wait. Otherwise, we timed out, so fail.
            if(postResetTapes.length() == 0) {
                handleProgrammerError("Bootloader dissappeared!");
                return;
            }

            // Try to create a new programmer by connecting to the port
            if(!programmer.open(postResetTapes.at(0))) {
                handleProgrammerError("could not connect to programmer!");
                return;
            }

            qDebug() << "Connected to programmer!";

            // Send Check Device Signature command
            programmer.checkDeviceSignature();

            // Queue all of the flash sections to memory
            while(!flashData.empty()) {
                FlashSection f = flashData.front();
                programmer.writeFlash(f.data, f.address);
                flashData.pop_front();
            }

            // TODO: Add verify stage?

            programmer.reset();

            state = State_Ready;
        }
        break;

    default:
        break;
    }

}
