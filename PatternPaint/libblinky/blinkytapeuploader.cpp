#include "blinkytapeuploader.h"

#include "avr109commands.h"
#include "avruploaddata.h"
#include "ProductionSketch.h"
#include "blinkycontroller.h"

#include <QDebug>

// TODO: Combine this with the definitions in avruploaddata.cpp
#define FLASH_BYTES_AVAILABLE           0x7000  // Amount of application space in the flash

#define EEPROM_TABLE_SIZE_BYTES         0x0010  // Amount of space available in the EEPROM

#define PAGE_SIZE_BYTES 128

/// Interval between polling the serial port list for new devices
#define BOOTLOADER_POLL_INTERVAL 200

/// Maximum time to wait before giving up on finding a bootloader
#define BOOTLOADER_POLL_TIMEOUT 10000

/// How long to wait between receiving notification that the programmer has been
/// reset, and notifying the caller that we are finished
#define BOOTLOADER_SETTLING_DELAY 500

/// Length of character buffer for debug messages
#define BUFF_LENGTH 100

BlinkyTapeUploader::BlinkyTapeUploader(QObject *parent) :
    BlinkyUploader(parent),
    state(State_Ready)
{
    connect(&commandQueue, SIGNAL(error(QString)),
            this, SLOT(handleError(QString)));
    connect(&commandQueue, SIGNAL(commandFinished(QString, QByteArray)),
            this, SLOT(handleCommandFinished(QString, QByteArray)));
}

QList<PatternWriter::Encoding> BlinkyTapeUploader::getSupportedEncodings() const
{
    QList<PatternWriter::Encoding> encodings;
    encodings.append(PatternWriter::RGB565_RLE);
    encodings.append(PatternWriter::RGB24);
    return encodings;
}

void BlinkyTapeUploader::cancel()
{
    if (state != State_WaitForBootloaderPort) {
        qDebug() << "Can't cancel during upload";
        return;
    }

    state = State_Ready;
}

void BlinkyTapeUploader::handleError(QString error)
{
    qCritical() << error;

    commandQueue.close();

    emit(finished(false));
}

void BlinkyTapeUploader::handleCommandFinished(QString command, QByteArray returnData)
{
    Q_UNUSED(returnData);

// qDebug() << "Command finished:" << command;
    setProgress(progress + 1);

    // we know reset is the last command, so the BlinkyTape should be ready soon.
    // Schedule a timer to emit the message shortly.
    // TODO: Let the receiver handle this instead.
    if (command == "reset") {
        commandQueue.close();
        emit(finished(true));
    }
}

void BlinkyTapeUploader::setProgress(int newProgress)
{
    qDebug() << "Progress:" << newProgress
             << "maxProgress:" << maxProgress;

    progress = newProgress;
    // TODO: Precalculate the max progress
    emit(progressChanged((progress*100)/maxProgress));
}

bool BlinkyTapeUploader::startUpload(BlinkyController &tape, QList<PatternWriter> &patternWriters)
{
    /// Create the compressed image and check if it will fit into the device memory
    avrUploadData data;
    if (!data.init(patternWriters)) {
        errorString = data.errorString;
        return false;
    }

    // TODO: Only write the sketch portion if we absolutely have to, to lower the risk
    // that we trash the firmware if there is a problem later

    // The entire sketch must fit into the available memory, minus a single page
    // at the end of flash for the configuration header
    // TODO: Verify this by looking at each section individually instead, making sure
    // none of them overlap or extend outside of available memory?
    if (data.sketch.length() + data.patternData.length() + data.patternTable.length()
        > FLASH_BYTES_AVAILABLE) {
        qCritical() << "sketch can't fit into memory!";

        errorString = QString(
            "Sorry! The Pattern is a bit too big to fit in BlinkyTape memory! Avaiable space=%1, Pattern size=%2")
                      .arg(FLASH_BYTES_AVAILABLE)
                      .arg(
            data.sketch.length() + data.patternData.length() + data.patternTable.length());
        return false;
    }

    // Put the sketch, pattern, and metadata into the programming queue.
    qDebug() << "Sketch address: " << data.sketchAddress << ", size: " << data.sketch.length();
    qDebug() << "pattern data address: " << data.patternDataAddress << ", size: "
             << data.patternData.length();
    qDebug() << "pattern table address: " << data.patternTableAddress << ", size: "
             << data.patternTable.length();

    flashData.push_back(FlashSection(data.sketchAddress, data.sketch));
    flashData.push_back(FlashSection(data.patternDataAddress, data.patternData));
    flashData.push_back(FlashSection(data.patternTableAddress, data.patternTable));

    return startUpload(tape);
}

bool BlinkyTapeUploader::upgradeFirmware(int timeout)
{
    QByteArray sketch = QByteArray(reinterpret_cast<const char *>(PRODUCTION_DATA),
                                   PRODUCTION_LENGTH);

    bootloaderPollTimeout = timeout;

    char buff[BUFF_LENGTH];
    snprintf(buff, BUFF_LENGTH, "Sketch size: %iB",
             sketch.length()),
    qDebug() << buff;

    // The entire sketch must fit into the available memory, minus a single page
    // at the end of flash for the configuration header
    // TODO: Could save ~100 bytes if we let the sketch spill into the unused portion
    // of the header.
    if (sketch.length() > FLASH_BYTES_AVAILABLE) {
        qDebug() << "sketch can't fit into memory!";

        errorString = QString(
            "Sorry! The Pattern is a bit too big to fit in BlinkyTape memory right now. We're working on improving this! Avaiable space=%1, Pattern size=%2")
                      .arg(FLASH_BYTES_AVAILABLE)
                      .arg(sketch.length());
        return false;
    }

    // Put the sketch, pattern, and metadata into the programming queue.
    flashData.push_back(FlashSection(PRODUCTION_ADDRESS, sketch));

    // TODO: This is duplicated in startUpload...
    maxProgress = 1;    // checkDeviceSignature
    foreach (FlashSection flashSection, flashData)
        maxProgress += 2*flashSection.data.count()/PAGE_SIZE_BYTES;
    setProgress(0);

    stateStartTime = QDateTime::currentDateTime();
    state = State_WaitForBootloaderPort;

    QTimer::singleShot(0, this, SLOT(doWork()));
    return true;
}

bool BlinkyTapeUploader::upgradeFirmware(BlinkyController &blinky)
{
    QByteArray sketch = QByteArray(reinterpret_cast<const char *>(PRODUCTION_DATA),
                                   PRODUCTION_LENGTH);

    char buff[BUFF_LENGTH];
    snprintf(buff, BUFF_LENGTH, "Sketch size: %iB",
             sketch.length()),
    qDebug() << buff;

    // The entire sketch must fit into the available memory, minus a single page
    // at the end of flash for the configuration header
    if (sketch.length() > FLASH_BYTES_AVAILABLE) {
        qDebug() << "sketch can't fit into memory!";

        errorString = QString(
            "Sorry! The Pattern is a bit too big to fit in BlinkyTape memory right now. We're working on improving this! Avaiable space=%1, Pattern size=%2")
                      .arg(FLASH_BYTES_AVAILABLE)
                      .arg(sketch.length());
        return false;
    }

    // Put the sketch, pattern, and metadata into the programming queue.
    flashData.push_back(FlashSection(PRODUCTION_ADDRESS, sketch));

    return startUpload(blinky);
}

bool BlinkyTapeUploader::startUpload(BlinkyController &blinky)
{
    maxProgress = 1;    // checkDeviceSignature
    foreach (FlashSection flashSection, flashData)
        maxProgress += 2*flashSection.data.count()/PAGE_SIZE_BYTES;

    setProgress(0);

    bootloaderPollTimeout = BOOTLOADER_POLL_TIMEOUT;

    // Now, start the polling processes to detect a new bootloader
    // We can't reset if we weren't already connected...
    if (!blinky.isConnected()) {
        errorString = "Not connected to a tape, cannot upload again";
        return false;
    }

    // Next, tell the tape to reset.
    blinky.reset();

    stateStartTime = QDateTime::currentDateTime();
    state = State_WaitForBootloaderPort;

    QTimer::singleShot(0, this, SLOT(doWork()));

    return true;
}

QString BlinkyTapeUploader::getErrorString() const
{
    return errorString;
}

void BlinkyTapeUploader::doWork()
{
    // TODO: This flow is really ungainly

    // qDebug() << "In doWork state=" << state;

    // Continue the current state
    switch (state) {
    case State_WaitForBootloaderPort:
    {
        // TODO: Only search BlinkyTapes bootloaders!
        QList<QSerialPortInfo> postResetTapes
            = BlinkyController::probeBootloaders();

        // If we didn't detect a bootloader and still have time, then queue the timer and
        // wait. Otherwise, we timed out, so fail.
        if (postResetTapes.count() == 0) {
            if ((bootloaderPollTimeout > 0)
                && (stateStartTime.msecsTo(QDateTime::currentDateTime())
                    > bootloaderPollTimeout)) {
                handleError("Timeout waiting for a bootloader device");
                return;
            }

            QTimer::singleShot(BOOTLOADER_POLL_INTERVAL, this, SLOT(doWork()));
            return;
        }

        qDebug() << "Bootloader waiting on: " << postResetTapes.at(0).portName();

        // Don't connect immediately, the device might need a short time to settle down
        QTimer::singleShot(BOOTLOADER_SETTLING_DELAY, this, SLOT(doWork()));
        state = State_WaitAfterBootloaderPort;

        break;
    }

    case State_WaitAfterBootloaderPort:
    {
        // TODO: Only search BlinkyTapes bootloaders!
        QList<QSerialPortInfo> postResetTapes
            = BlinkyController::probeBootloaders();

        // If we didn't detect a bootloader and still have time, then queue the timer and
        // wait. Otherwise, we timed out, so fail.
        if (postResetTapes.count() == 0) {
            handleError("Bootloader dissappeared!");
            return;
        }

        // Try to create a new programmer by connecting to the port
        if (!commandQueue.open(postResetTapes.at(0))) {
            handleError("could not connect to programmer!");
            return;
        }

        qDebug() << "Connected to programmer!";

        // Send Check Device Signature command
        commandQueue.enqueue(Avr109Commands::checkDeviceSignature());

        // Queue an EEPROM clear
        QByteArray eepromBytes(EEPROM_TABLE_SIZE_BYTES, char(255));
        commandQueue.enqueue(Avr109Commands::writeEeprom(eepromBytes,0));
        // TODO: Verify EEPROM?

        // Queue all of the flash sections to memory
        while (!flashData.empty()) {
            FlashSection f = flashData.front();
            commandQueue.enqueue(Avr109Commands::writeFlash(f.data, f.address));
            commandQueue.enqueue(Avr109Commands::verifyFlash(f.data, f.address));
            flashData.pop_front();
        }

        commandQueue.enqueue(Avr109Commands::reset());

        state = State_Ready;
        break;
    }

    default:
        break;
    }
}
