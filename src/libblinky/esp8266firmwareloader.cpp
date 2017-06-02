#include "esp8266firmwareloader.h"

#include "blinkycontroller.h"
#include "esp8266bootloadercommands.h"
#include "bytearrayhelpers.h"

#define BOOTLOADER_SYNC_FIRST_INTERVAL 200  // If this is low, we get an unexpected response to the sync command.
#define BOOTLOADER_SYNC_INTERVAL 200  // If this is low, we get an unexpected response to the sync command.
#define BOOTLOADER_RESET_LOOP_RETRIES 10
#define BOOTLOADER_SYNC_RETRIES 2

#define EIGHTBYEIGHT_DEFAULT_FIRMWARE_LOCATION ":/firmware/eightbyeight/default/espfirmware.bin"

#define ESP_BLOCK_SIZE 1024

Esp8266FirmwareLoader::Esp8266FirmwareLoader(QObject *parent) :
    FirmwareLoader(parent)
{
    connect(&commandQueue, SerialCommandQueue::errorOccured,
            this, Esp8266FirmwareLoader::handleError);
    connect(&commandQueue, SerialCommandQueue::commandFinished,
            this, Esp8266FirmwareLoader::handleCommandFinished);
    connect(&commandQueue, SerialCommandQueue::lastCommandFinished,
            this, Esp8266FirmwareLoader::handleLastCommandFinished);
}

bool Esp8266FirmwareLoader::updateFirmware(BlinkyController &controller)
{
    QFile file(EIGHTBYEIGHT_DEFAULT_FIRMWARE_LOCATION);
    if (!file.open(QIODevice::ReadOnly)) {
        errorString = "Cannot open firmware file";
        return false;
    }
    QByteArray data = file.readAll();

    ByteArrayHelpers::padToBoundary(data, ESP_BLOCK_SIZE);
    flashData = ByteArrayHelpers::chunkData(data, ESP_BLOCK_SIZE);

    progress = 0;
    maxProgress = 2 + 2 + flashData.length();

    controller.getPortInfo(serialPortInfo);

    if (controller.isConnected())
        controller.close();

    resetLoopsRemaining = BOOTLOADER_RESET_LOOP_RETRIES;
    state = State_startReset;
    doWork();

    return true;
}

bool Esp8266FirmwareLoader::restoreFirmware(qint64 timeout)
{
    Q_UNUSED(timeout);

    errorString = "Firmware update not currently supported for Esp8266!";
    return false;
}

QString Esp8266FirmwareLoader::getErrorString() const
{
    return errorString;
}

void Esp8266FirmwareLoader::cancel()
{
    handleError("Firmware update cancelled");
}

void Esp8266FirmwareLoader::doWork()
{
    switch (state) {
    case State_startReset:
    {
        if(commandQueue.isOpen())
            commandQueue.close();

        serialPort.setPort(serialPortInfo);
        serialPort.open(QIODevice::ReadWrite);

        state = State_assertRTS;
        QTimer::singleShot(1, this, SLOT(doWork()));

        break;
    }
    case State_assertRTS:
    {
        serialPort.setRequestToSend(true);
        serialPort.setDataTerminalReady(false);

        state = State_assertDTR;

        QTimer::singleShot(1, this, SLOT(doWork()));

        break;
    }
    case State_assertDTR:
    {
        serialPort.setRequestToSend(false);
        serialPort.setDataTerminalReady(true);

        state = State_releaseBootPins;
        QTimer::singleShot(100, this, SLOT(doWork()));

        break;
    }
    case State_releaseBootPins:
    {
        serialPort.setRequestToSend(false);
        serialPort.setDataTerminalReady(false);

        state = State_connectCommandQueue;;
        QTimer::singleShot(300, this, SLOT(doWork()));

        break;
    }
    case State_connectCommandQueue:
    {
        serialPort.close();
        commandQueue.open(serialPortInfo);

        syncTriesRemaining = BOOTLOADER_SYNC_RETRIES;
        state = State_startBootloaderSync;
        QTimer::singleShot(BOOTLOADER_SYNC_FIRST_INTERVAL, this, SLOT(doWork()));

        break;
    }
    case State_startBootloaderSync:
    {
        state = State_waitForBootloaderSync;
        commandQueue.enqueue(Esp8266BootloaderCommands::SyncFrame());

        break;
    }
    case State_waitForBootloaderSync:
    {
        state = State_doFlashDownload;

        unsigned int magicLength = 217088;  // TODO: Why this length and not the original data length?

        commandQueue.enqueue(Esp8266BootloaderCommands::flashDownloadStart(
                                 magicLength, flashData.length(), ESP_BLOCK_SIZE, 0));

        unsigned int sequence = 0;
        for(QByteArray chunk : flashData) {
            commandQueue.enqueue(Esp8266BootloaderCommands::flashDownloadData(
                                     sequence, chunk));
            sequence++;
        }

        commandQueue.enqueue(Esp8266BootloaderCommands::flashDownloadFinish(
                                 0));
        break;
    }
    case State_doFlashDownload:
    {
        state = State_Done;
        QMetaObject::invokeMethod(this, "doWork", Qt::QueuedConnection);

        break;
    }
    case State_Done:
    {
        commandQueue.close();
        emit(finished(true));

        break;
    }

    }
}

void Esp8266FirmwareLoader::handleError(QString error)
{
    qCritical() << error;

    if(state == State_waitForBootloaderSync) {
        if(syncTriesRemaining > 0) {
            syncTriesRemaining--;
            state = State_startBootloaderSync;
            QTimer::singleShot(BOOTLOADER_SYNC_INTERVAL, this, SLOT(doWork()));
            return;
        }
        else if(resetLoopsRemaining > 0) {
            resetLoopsRemaining--;

            state = State_startReset;
            QTimer::singleShot(0, this, SLOT(doWork()));
            return;
        }
    }

    errorString = error;

    if(serialPort.isOpen())
        serialPort.close();

    if(commandQueue.isOpen())
        commandQueue.close();

    emit(finished(false));
}

void Esp8266FirmwareLoader::handleCommandFinished(QString command, QByteArray returnData)
{
    Q_UNUSED(command);
    Q_UNUSED(returnData);

    setProgress(progress + 1);
}

void Esp8266FirmwareLoader::handleLastCommandFinished()
{
    doWork();
}

void Esp8266FirmwareLoader::setProgress(int newProgress)
{
    progress = newProgress;

    // Clip the progress so that it never reaches 100%.
    // It will be closed by the finished() signal.
    if (progress >= maxProgress)
        maxProgress = progress + 1;

    int progressPercent = (progress*100)/maxProgress;

    emit(progressChanged(progressPercent));
}
