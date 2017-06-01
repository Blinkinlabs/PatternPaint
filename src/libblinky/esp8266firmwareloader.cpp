#include "esp8266firmwareloader.h"

#include "blinkycontroller.h"
#include "esp8266bootloadercommands.h"
#include "bytearrayhelpers.h"

#define BOOTLOADER_SYNC_FIRST_INTERVAL 500  // If this is low, we get an unexpected response to the sync command.
#define BOOTLOADER_SYNC_INTERVAL 200  // If this is low, we get an unexpected response to the sync command.
#define BOOTLOADER_SYNC_RETRIES 5


#define ESP_BLOCK_SIZE 1024

Esp8266FirmwareLoader::Esp8266FirmwareLoader(QObject *parent) :
    BlinkyUploader(parent)
{
    connect(&commandQueue, SIGNAL(error(QString)),
            this, SLOT(handleError(QString)));
    connect(&commandQueue, SIGNAL(commandFinished(QString, QByteArray)),
            this, SLOT(handleCommandFinished(QString, QByteArray)));
    connect(&commandQueue, SIGNAL(lastCommandFinished()),
            this, SLOT(handleLastCommandFinished()));
}

bool Esp8266FirmwareLoader::storePatterns(BlinkyController &controller, QList<PatternWriter> &patternWriters)
{
    errorString = "Store patterns not supported for Esp8266!";
    return false;
}

bool Esp8266FirmwareLoader::updateFirmware(BlinkyController &controller)
{
    controller.getPortInfo(serialPortInfo);

    if (controller.isConnected())
        controller.close();

    serialPort.setPort(serialPortInfo);
    serialPort.open(QIODevice::ReadWrite);

    state = State_assertRTS;
    doWork();

    return true;
}

bool Esp8266FirmwareLoader::restoreFirmware(qint64 timeout)
{
    errorString = "Firmware update not currently supported for Esp8266!";
    return false;
}

QString Esp8266FirmwareLoader::getErrorString() const
{
    return errorString;
}

QList<PatternWriter::Encoding> Esp8266FirmwareLoader::getSupportedEncodings() const
{
    QList<PatternWriter::Encoding> encodings;
    return encodings;
}

void Esp8266FirmwareLoader::cancel()
{
    handleError("Firmware update cancelled");
}

void Esp8266FirmwareLoader::doWork()
{
    qDebug() << "In doWork state=" << state;

    switch (state) {
    case State_assertRTS:
    {
        serialPort.setRequestToSend(true);
        serialPort.setDataTerminalReady(false);

        state = State_assertDTR;
//        QTimer::singleShot(0, this, SLOT(doWork()));
        QMetaObject::invokeMethod(this, "doWork", Qt::QueuedConnection);


        break;
    }
    case State_assertDTR:
    {
        serialPort.setRequestToSend(false);
        serialPort.setDataTerminalReady(true);

        state = State_releaseBootPins;
//        QTimer::singleShot(0, this, SLOT(doWork()));
        QMetaObject::invokeMethod(this, "doWork", Qt::QueuedConnection);

        break;
    }
    case State_releaseBootPins:
    {
        serialPort.setRequestToSend(false);
        serialPort.setDataTerminalReady(false);

        state = State_connectCommandQueue;
//        QTimer::singleShot(0, this, SLOT(doWork()));
        QMetaObject::invokeMethod(this, "doWork", Qt::QueuedConnection);

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

        QFile file(":/firmware/eightbyeight/default/espfirmware.bin");
        if (!file.open(QIODevice::ReadOnly)) return;
        QByteArray data = file.readAll();

        ByteArrayHelpers::padToBoundary(data, ESP_BLOCK_SIZE);
        QList<QByteArray> chunks = ByteArrayHelpers::chunkData(data, ESP_BLOCK_SIZE);

        unsigned int magicLength = 217088;  // TODO: Why this length and not the original data length?

        commandQueue.enqueue(Esp8266BootloaderCommands::flashDownloadStart(
                                 magicLength, chunks.length(), ESP_BLOCK_SIZE, 0));

        unsigned int sequence = 0;
        for(QByteArray chunk : chunks) {
            commandQueue.enqueue(Esp8266BootloaderCommands::flashDownloadData(
                                     sequence, chunk));
            sequence++;
        }

        commandQueue.enqueue(Esp8266BootloaderCommands::flashDownloadFinish(
                                 1));
        break;
    }
    case State_doFlashDownload:
    {
        state = State_Done;

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
        if(syncTriesRemaining > 1) {
            syncTriesRemaining--;
            state = State_startBootloaderSync;
            QTimer::singleShot(BOOTLOADER_SYNC_INTERVAL, this, SLOT(doWork()));
            return;
        }
    }

    errorString = error;

    if(serialPort.isOpen())
        serialPort.close();

    if(commandQueue.isConnected())
        commandQueue.close();

    emit(finished(false));
}

void Esp8266FirmwareLoader::handleCommandFinished(QString command, QByteArray returnData)
{
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
