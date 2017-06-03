#include "avr109firmwareloader.h"

#include "avr109commands.h"
#include "blinkycontroller.h"
#include "blinkytapeuploaddata.h"
#include "firmwarestore.h"

#include <QDebug>

// TODO: Combine this with the definitions in avruploaddata.cpp
#define EEPROM_TABLE_SIZE_BYTES         0x0010  // Amount of space available in the EEPROM

/// How long to delay between resetting the blinkyController and probing for a bootloader
#define BLINKY_RESET_DELAY 500

/// Interval between polling the serial port list for new devices (milliseconds)
#define BOOTLOADER_POLL_INTERVAL 200

/// Maximum time to wait before giving up on finding a bootloader (milliseconds)
#define BOOTLOADER_POLL_TIMEOUT 10000

/// How long to wait between receiving notification that the programmer has been
/// reset, and notifying the caller that we are finished (milliseconds)
#define BOOTLOADER_SETTLING_DELAY 500

/// Maximum number of times to try writing the flash memory
#define FLASH_WRITE_MAX_RETRIES 10


Avr109FirmwareLoader::Avr109FirmwareLoader(QObject *parent) :
    FirmwareLoader(parent)
{
    connect(&commandQueue, &SerialCommandQueue::errorOccured,
            this, &Avr109FirmwareLoader::handleError);
    connect(&commandQueue, &SerialCommandQueue::commandFinished,
            this, &Avr109FirmwareLoader::handleCommandFinished);
    connect(&commandQueue, &SerialCommandQueue::lastCommandFinished,
            this, &Avr109FirmwareLoader::handleLastCommandFinished);
}

void Avr109FirmwareLoader::cancel()
{
    // TODO: This doesn't seem like it will function correctly

    if (state != State_ProbeBootloaders) {
        qDebug() << "Can't cancel during upload";
        return;
    }

    state = State_SendingCommands;
}

void Avr109FirmwareLoader::setProgress(int newProgress)
{
    progress = newProgress;

    // Clip the progress so that it never reaches 100%.
    // It will be closed by the finished() signal.
    if (progress >= maxProgress)
        maxProgress = progress + 1;

    int progressPercent = (progress*100)/maxProgress;

    emit(progressChanged(progressPercent));
}

void Avr109FirmwareLoader::setDialogText()
{

    QSettings settings;
    QString firmwareName = settings.value("BlinkyTape/firmwareName", BLINKYTAPE_DEFAULT_FIRMWARE_NAME).toString();
    int flashUsed = 0;

    QString textLabel;
    textLabel.append("Saving to Blinky...\n");
    textLabel.append("\n");
    textLabel.append("Firmware: " + firmwareName + "\n");

    for (MemorySection& section : flashData)
        flashUsed += section.data.length();

    float flashUsedPercent = float(flashUsed)*100/FLASH_MEMORY_AVAILABLE;
    textLabel.append(QString("Flash used: %1%").arg(QString::number(flashUsedPercent,'f', 1)));

    emit(setText(textLabel));
}

// TODO: Remove this in favor of a global bootloader search utility.
bool Avr109FirmwareLoader::restoreFirmware(qint64 timeout)
{
    MemorySection sketch = FirmwareStore::getFirmwareData(BLINKYTAPE_FACTORY_FIRMWARE_NAME);

    if(sketch.data.isNull()) {
        qDebug() << "Error loading factory firmware!";
        return false;
    }

    // Put the sketch, pattern, and metadata into the programming queue.
    flashData.append(sketch);

    if(!checkFirmwareFits())
        return false;

    probeBootloaderTimeout = timeout;

    reallyStartUpload();
    return true;
}


// TODO: Remove these, require the caller to provide the firmware image.
bool Avr109FirmwareLoader::updateFirmware(BlinkyController &blinkyController)
{
    MemorySection sketch = FirmwareStore::getFirmwareData(BLINKYTAPE_FACTORY_FIRMWARE_NAME);

    if(sketch.data.isNull()) {
        qDebug() << "Error loading factory firmware!";
        return false;
    }

    QList<MemorySection> firmware;
    firmware.append(sketch);

    return updateFirmware(blinkyController, firmware);
}

bool Avr109FirmwareLoader::updateFirmware(BlinkyController &blinkyController, QList<MemorySection> firmware)
{
    flashData = firmware;

    if(!checkFirmwareFits())
        return false;

    // Now, start the polling processes to detect a new bootloader
    // We can't reset if we weren't already connected...
    if (!blinkyController.isConnected()) {
        errorString = "Not connected to a tape, cannot upload again";
        return false;
    }

    probeBootloaderTimeout = BOOTLOADER_POLL_TIMEOUT;

    // Next, tell the tape to reset.
    blinkyController.reset();

    // TODO: Add a new state to check that the reset actually happened, before continuing.
    QTimer::singleShot(BLINKY_RESET_DELAY, this, SLOT(reallyStartUpload()));
    return true;
}


void Avr109FirmwareLoader::reallyStartUpload() {

    maxProgress = 1;    // checkDeviceSignature
    maxProgress += 2;   // erase EEPROM
    maxProgress += 1;   // reset

    // There are 4 commands for each page-
    // setaddress, writeflashpage, setaddress, verifyflashpage
    for (MemorySection flashSection : flashData)
        maxProgress += 4*flashSection.data.count()/FLASH_MEMORY_PAGE_SIZE_BYTES;

    setProgress(0);

    probeBootloaderStartTime = QDateTime::currentDateTime();

    flashWriteRetriesRemaining = FLASH_WRITE_MAX_RETRIES;

    state = State_ProbeBootloaders;
    QTimer::singleShot(0, this, SLOT(doWork()));
}


void Avr109FirmwareLoader::handleError(QString error)
{
    qCritical() << error;

    // If we're in writeflashdata and the error was, try re-starting the upload process
    if(state == State_WriteFlashData) {
        if((error == "Got unexpected data back")
                && (flashWriteRetriesRemaining > 0)) {
            qCritical() << "Unexpected data error- re-starting firmware upload process";

            flashWriteRetriesRemaining--;

            // Reset the progress to the beginning of the flash write section
            progress = 3;

            commandQueue.flushQueue();
            doWork();

            return;
        }
    }

    // Otherwise we can't recover
    commandQueue.close();
    emit(finished(false));
}

void Avr109FirmwareLoader::handleCommandFinished(QString command, QByteArray returnData)
{
    Q_UNUSED(command);
    Q_UNUSED(returnData);

    setProgress(progress + 1);
}

void Avr109FirmwareLoader::handleLastCommandFinished()
{
    qDebug() << "All commands finished! Transitioning to next state";

    // TODO: Moveme to doWork() ?
    switch (state) {
    case State_InitializeBootloader:
        state = State_WriteFlashData;
        doWork();
        break;

    case State_WriteFlashData:
        state = State_ResetBootloader;
        doWork();
        break;

    case State_ResetBootloader:
        commandQueue.close();
        emit(finished(true));
        break;

    default:
        qCritical() << "Got a last command finished signal when not expected";
        break;
    }
}

bool Avr109FirmwareLoader::checkFirmwareFits()
{
    for (MemorySection& section : flashData) {
        qDebug() << "Flash Section:" << section.name
                 << "address: " << section.address
                 << "size: " << section.data.length();
    }

    // TODO: Check if all flash sections fit in memory
    // TODO: Check that all sections are not overlapping
    int flashUsed = 0;
    for (MemorySection& section : flashData)
        flashUsed += section.data.length();
    int flashTotal = FLASH_MEMORY_AVAILABLE;

    if (flashUsed > flashTotal) {
        errorString = "Pattern data too large, cannot fit on device!\n";
        errorString.append("Try removing some patterns, or making them shorter\n");
        errorString.append("Pattern size: " + QString::number(flashUsed) + " bytes\n");
        errorString.append("Available space: " + QString::number(flashTotal) + " bytes");

        return false;
    }

    return true;
}

QString Avr109FirmwareLoader::getErrorString() const
{
    return errorString;
}

// TODO: Drop this, and put into a different class.
bool Avr109FirmwareLoader::bootloaderAvailable() {
    return !BlinkyController::probeBootloaders().empty();
}

bool Avr109FirmwareLoader::ProbeBootloaderTimeout() {
    // -1 is a special case to never time out.
    if(probeBootloaderTimeout == -1) {
        return false;
    }

    return (probeBootloaderStartTime.msecsTo(QDateTime::currentDateTime()) > probeBootloaderTimeout);
}

void Avr109FirmwareLoader::doWork()
{
    // TODO: This flow is really ungainly

    // qDebug() << "In doWork state=" << state;

    // Continue the current state
    switch (state) {
    case State_ProbeBootloaders:
    {
        // TODO: Only search BlinkyTapes bootloaders!

        // If we ran out of time, error
        if(ProbeBootloaderTimeout()) {
            handleError("Timeout waiting for a bootloader device");
            return;
        }

        // If we're still waiting for a bootloader, set a timer to look again
        // after a small interval
        if(!bootloaderAvailable()) {
            QTimer::singleShot(BOOTLOADER_POLL_INTERVAL, this, SLOT(doWork()));
            return;
        }

        // Otherwise we found a bootloader.
        // Don't connect immediately, the device might need a short time to settle down
        QTimer::singleShot(BOOTLOADER_SETTLING_DELAY, this, SLOT(doWork()));
        state = State_InitializeBootloader;

        break;
    }

    case State_InitializeBootloader:
    {
        // TODO: Only search BlinkyTapes bootloaders!
        QList<QSerialPortInfo> bootloaders
            = BlinkyController::probeBootloaders();

        // If we didn't detect a bootloader and still have time, then queue the timer and
        // wait. Otherwise, we timed out, so fail.
        if (bootloaders.count() == 0) {
            handleError("Bootloader dissappeared!");
            return;
        }

        // Try to create a new programmer by connecting to the port
        if (!commandQueue.open(bootloaders.front())) {
            handleError("could not connect to programmer!");
            return;
        }

        qDebug() << "Connected to bootloader!";

        setDialogText();

        // Send Check Device Signature command
        commandQueue.enqueue(Avr109Commands::checkDeviceSignature());

        // Queue an EEPROM clear
        QByteArray eepromBytes(EEPROM_TABLE_SIZE_BYTES, char(255));
        commandQueue.enqueue(Avr109Commands::writeEeprom(eepromBytes,0));

        // TODO: Verify EEPROM?

        break;
    }

    case State_WriteFlashData:
    {
        // Queue all of the flash sections to memory
        for(MemorySection& section : flashData) {
            // TODO: How to report errors in command formation?
            commandQueue.enqueue(Avr109Commands::writeFlash(section.data, section.address));
            commandQueue.enqueue(Avr109Commands::verifyFlash(section.data, section.address));
        }

        break;
    }

    case State_ResetBootloader:
    {
        commandQueue.enqueue(Avr109Commands::reset());

        break;
    }

    case State_SendingCommands:
    default:
        break;
    }
}
