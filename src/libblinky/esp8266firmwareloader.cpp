#include "esp8266firmwareloader.h"

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
    errorString = "Firmware update not currently supported for Esp8266!";
    return false;

//    QSerialPortInfo info;
//    controller.getPortInfo(info);

//    if (controller.isConnected())
//        controller.close();

//    commandQueue.open(info);
//    state = State_checkFirmwareVersion;
//    doWork();
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
    qDebug() << "Cancel signalled, but not supported";
}

void Esp8266FirmwareLoader::doWork()
{
//QTimer::singleShot(0, this, SLOT(doWork()));
}

void Esp8266FirmwareLoader::handleError(QString error)
{
    errorString = error;
}

void Esp8266FirmwareLoader::handleCommandFinished(QString command, QByteArray returnData)
{

}

void Esp8266FirmwareLoader::handleLastCommandFinished()
{

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
