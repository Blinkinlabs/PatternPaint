#include "blinkypendant.h"
#include "blinkypendantuploader.h"
#include <QDebug>

/// Interval between scans to see if the device is still connected
#define CONNECTION_SCANNER_INTERVAL 100

#define PENDANT_MAX_PIXELS 10


// TODO: Support a method for loading these from preferences file
QList<QSerialPortInfo> BlinkyPendant::probe()
{
    QList<QSerialPortInfo> serialPorts = QSerialPortInfo::availablePorts();
    QList<QSerialPortInfo> tapes;

    foreach (const QSerialPortInfo &info, serialPorts) {
        // Only connect to known BlinkyPendants
        if(info.vendorIdentifier() == BLINKYPENDANT_SKETCH_VID
           && info.productIdentifier() == BLINKYPENDANT_SKETCH_PID) {
            tapes.push_back(info);
        }
    }

    return tapes;
}

// TODO: Support a method for loading these from preferences file
QList<QSerialPortInfo> BlinkyPendant::probeBootloaders()
{
    QList<QSerialPortInfo> serialPorts = QSerialPortInfo::availablePorts();
    QList<QSerialPortInfo> tapes;

    foreach (const QSerialPortInfo &info, serialPorts) {
        // Only connect to known BlinkyPendants
        if(info.vendorIdentifier() == BLINKYPENDANT_BOOTLOADER_VID
           && info.productIdentifier() == BLINKYPENDANT_BOOTLOADER_PID) {
            tapes.push_back(info);
        }
    }

    return tapes;
}


BlinkyPendant::BlinkyPendant(QObject *parent) :
    BlinkyController(parent)
{
    serial = new QSerialPort(this);
    serial->setSettingsRestoredOnClose(false);

    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)),
            this, SLOT(handleSerialError(QSerialPort::SerialPortError)));

    connect(serial, SIGNAL(readyRead()), this, SLOT(handleSerialReadData()));

    // Windows doesn't notify us if the tape was disconnected, so we have to check peroidically
    #if defined(Q_OS_WIN)
    connectionScannerTimer = new QTimer(this);
    connectionScannerTimer->setInterval(CONNECTION_SCANNER_INTERVAL);
    connect(connectionScannerTimer, SIGNAL(timeout()), this, SLOT(connectionScannerTimer_timeout()));
    #endif
}

void BlinkyPendant::handleSerialError(QSerialPort::SerialPortError error)
{
    // The serial library appears to emit an extraneous SerialPortError
    // when open() is called. Just ignore it.
    if(error == QSerialPort::NoError) {
        return;
    }

    QString errorString = serial->errorString();


    if (error == QSerialPort::ResourceError) {
        qCritical() << "Serial resource error, BlinkyPendant unplugged?" << errorString;
    }
    else {
        qCritical() << "Unrecognized serial error:" << errorString;
    }

    close();
}

#if defined(Q_OS_WIN)
void BlinkyPendant::connectionScannerTimer_timeout() {
    // If we are already disconnected, disregard.
    if(!isConnected()) {
        connectionScannerTimer->stop();
        return;
    }

    // Check if our serial port is on the list
    QSerialPortInfo currentInfo = QSerialPortInfo(*serial);

    QList<QSerialPortInfo> tapes = findBlinkyPendants();
    foreach (const QSerialPortInfo &info, tapes) {
        // If we get a match, reset the timer and return.
        // We consider it a match if the port is the same on both
        if(info.portName() == currentInfo.portName()) {
            return;
        }
    }

    // We seem to have lost our port, bail
    close();
}
#endif

bool BlinkyPendant::open(QSerialPortInfo info) {
    if(isConnected()) {
        qCritical() << "Already connected to a BlinkyPendant";
        return false;
    }

    qDebug() << "Connecting to BlinkyPendant on " << info.portName();

#if defined(Q_OS_OSX)
    // Note: This should be info.portName(). Changed here as a workaround for:
    // https://bugreports.qt.io/browse/QTBUG-45127
    serial->setPortName(info.systemLocation());
#else
    serial->setPortName(info.portName());
#endif
    serial->setBaudRate(QSerialPort::Baud115200);

    if( !serial->open(QIODevice::ReadWrite) ) {
        qDebug() << "Could not connect to BlinkyPendant. Error: " << serial->error() << serial->errorString();
        return false;
    }

    serialInfo = info;

    emit(connectionStatusChanged(true));

#if defined(Q_OS_WIN)
    // Schedule the connection scanner
    connectionScannerTimer->start();
#endif
    return true;
}

void BlinkyPendant::close() {
    if(serial->isOpen()) {
        serial->close();
    }

    emit(connectionStatusChanged(isConnected()));
}

void BlinkyPendant::handleSerialReadData()
{
//    qDebug() << "Got data from BlinkyPendant, discarding.";
    // Discard any data we get back from the BlinkyPendant
    serial->readAll();
}


bool BlinkyPendant::isConnected() {
    return serial->isOpen();
}

void BlinkyPendant::sendUpdate(QByteArray LedData)
{
    if(!isConnected()) {
        qCritical() << "Strip not connected, not sending update!";
        return;
    }

    // If there is data pending to send, skip this update to prevent overflowing
    // the buffer.
    if(serial->bytesToWrite() >0) {
//        qDebug() << "Output data still in buffer, dropping this update frame";
        return;
    }

    // Fit the data to the output device
    LedData = LedData.leftJustified(PENDANT_MAX_PIXELS*3, (char)0x00, true);

    // Trim anything that's 0xff
    for(int i = 0; i < LedData.length(); i++) {
        if(LedData[i] == (char)255) {
            LedData[i] = 254;
        }
    }

    // Append an 0xFF to signal the flip command
    LedData.append(0xFF);

    int writeLength = serial->write(LedData);
    if(writeLength != LedData.length()) {
        qCritical() << "Error writing all the data out, expected:" << LedData.length()
                    << ", wrote:" << writeLength;
    }
}

bool BlinkyPendant::getPortInfo(QSerialPortInfo& info)
{
    if(!isConnected()) {
        return false;
    }

    info = serialInfo;
    return true;
}

void BlinkyPendant::reset()
{
    // Nothing to reset.
}


bool BlinkyPendant::getUploader(QPointer<PatternUploader>& uploader)
{
    if(!isConnected()) {
        return false;
    }

    uploader = new BlinkyPendantUploader(parent());

    return true;
}
