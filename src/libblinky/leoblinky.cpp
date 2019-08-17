#include "leoblinky.h"
#include "leoblinkyuploader.h"

#include <QDebug>
#include <QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>

/// Interval between scans to see if the device is still connected
#define CONNECTION_SCANNER_INTERVAL 100

#define MIN_UPDATE_INTERVAL 3

#define WRITE_BUSY_DELAY 2
#define WRITE_CHUNK_DELAY 4
#define CHUNK_SIZE 300

LeoBlinky::LeoBlinky(QSerialPortInfo info, QObject *parent) :
    BlinkyController(parent),
    serialInfo(info)
{
    serial = new QSerialPort(this);
    serial->setSettingsRestoredOnClose(false);

    connect(serial, &QSerialPort::errorOccurred,
            this, &LeoBlinky::handleError);

    connect(serial, &QSerialPort::readyRead,
            this, &LeoBlinky::handleReadData);

    serialWriteTimer.setSingleShot(true);
    connect(&serialWriteTimer, SIGNAL(timeout()), this, SLOT(sendChunk()));

#if defined(CONNECTION_SCANNER_TIMER)
    connectionScannerTimer.setInterval(CONNECTION_SCANNER_INTERVAL);
    connect(&connectionScannerTimer, SIGNAL(timeout()), this,
            SLOT(connectionScannerTimer_timeout()));
#endif
}

QString LeoBlinky::getName() const
{
    return QString("LeoBlinky");
}

void LeoBlinky::handleError(QSerialPort::SerialPortError error)
{
    // The serial library appears to emit an extraneous SerialPortError
    // when open() is called. Just ignore it.
    if (error == QSerialPort::NoError)
        return;

    QString errorString = serial->errorString();

    if (error == QSerialPort::ResourceError)
        qCritical() << "Serial resource error, device unplugged?" << errorString;
    else
        qCritical() << "Unrecognized serial error:" << errorString;

    close();
}

void LeoBlinky::sendChunk()
{
    // If the last bit didn't get written yet, try again shortly.
    if (serial->bytesToWrite() > 0) {
        qDebug() << "Busy, trying again shortly";
        serialWriteTimer.start(WRITE_BUSY_DELAY);
        return;
    }

    while (!chunks.isEmpty()) {
        QByteArray chunk = chunks.takeFirst();

        int writeLength = serial->write(chunk);
        if (writeLength != chunk.length()) {
            qCritical() << "Error writing all the data out, expected:" << chunk.length()
                        << ", wrote:" << writeLength;
        }
        serial->flush();

        if (!chunks.isEmpty()) {
            serialWriteTimer.start(WRITE_CHUNK_DELAY);
            return;
        }
    }
}

#if defined(CONNECTION_SCANNER_TIMER)
void LeoBlinky::connectionScannerTimer_timeout()
{
    // If we are already disconnected, disregard.
    if (!isConnected()) {
        connectionScannerTimer.stop();
        return;
    }

    // Check if our serial port is on the list
    QSerialPortInfo currentInfo = QSerialPortInfo(*serial);

    QList<QPointer<ControllerInfo> > tapes = probe();
    foreach (ControllerInfo *info, tapes) {
        // If we get a match, reset the timer and return.
        // We consider it a match if the port is the same on both
        if (info->resourceName() == currentInfo.portName())
            return;
    }

    // We seem to have lost our port, bail
    close();
}

#endif

bool LeoBlinky::open()
{
    if (isConnected()) {
        qCritical() << "Already connected to a device";
        return false;
    }

    qDebug() << "Connecting to device on " << serialInfo.portName();

    serial->setPortName(serialInfo.portName());
    serial->setBaudRate(QSerialPort::Baud115200);

    if (!serial->open(QIODevice::ReadWrite)) {
        qDebug() << "Could not connect to device. Error: " << serial->error()
                 << serial->errorString();
        return false;
    }

    emit(connectionStatusChanged(true));

#if defined(CONNECTION_SCANNER_TIMER)
    // Schedule the connection scanner
    connectionScannerTimer.start();
#endif
    return true;
}

void LeoBlinky::close()
{
    if (isConnected())
        serial->close();

    emit(connectionStatusChanged(isConnected()));
}

void LeoBlinky::handleReadData()
{
    // Discard any data we get back from the LeoBlinky
    serial->readAll();
}

bool LeoBlinky::isConnected()
{
    return serial->isOpen();
}

void LeoBlinky::sendUpdate(QByteArray ledData)
{
    if (!isConnected()) {
        qCritical() << "Controller not connected, not sending update!";
        return;
    }

    // If there is data pending to send, skip this update to prevent overflowing
    // the buffer.
    if (!chunks.isEmpty()) {
        qDebug() << "Output data still in buffer, dropping this update frame";
        return;
    }

    QByteArray outputData;
    outputData.append(char(0xde));    // Header (2 bytes, MSB first)
    outputData.append(char(0xad));
    outputData.append(char(0));       // Size (2 bytes, MSB first)
    outputData.append(char(ledData.length()/3+1));
    outputData.append(char(0x00));    // Command: update display

    // TODO: Duplicated in LeoBlinkyUploader::StartUpload()
    for(int i = 0; i < ledData.length()/3; i++) {
        // ITU BT.709 RGB to luma conversion:
        int val = static_cast<int>((0.2126*uint8_t(ledData[i*3])
                                   + 0.7152*uint8_t(ledData[i*3+1])
                                   + 0.0722*uint8_t(ledData[i*3+2])));

        outputData.append(char(val));    // Just use the red channel for now
    }

    outputData.append(char(0x00));    // CRC (unused)

    for (int chunk = 0; chunk *CHUNK_SIZE < outputData.length(); chunk++) {
        int chunkLength = CHUNK_SIZE;
        if ((chunk+1)*CHUNK_SIZE > outputData.length())
            chunkLength = outputData.length() - chunk*CHUNK_SIZE;

        QByteArray chunkData = outputData.mid(chunk*CHUNK_SIZE, chunkLength);
        chunks.append(chunkData);

// qDebug() << "size=" << outputData.length() << " chunk=" << chunk << " position=" << chunk*CHUNK_SIZE << " length=" << chunkLength;
// qDebug() << chunkData.toHex();
    }

// qDebug() << "Total chunks:" << chunks.size();
    sendChunk();
}

bool LeoBlinky::getPortInfo(QSerialPortInfo &info)
{
    if (!isConnected())
        return false;

    info = serialInfo;
    return true;
}

void LeoBlinky::reset()
{
    qDebug() << "Reset not supported";
}

bool LeoBlinky::getPatternUploader(QPointer<BlinkyUploader> &uploader)
{
    if (!isConnected())
        return false;

    uploader = new LeoBlinkyUploader(parent());
    return true;
}

bool LeoBlinky::getFirmwareLoader(QPointer<FirmwareLoader> & /*loader*/)
{
    return false;
}
