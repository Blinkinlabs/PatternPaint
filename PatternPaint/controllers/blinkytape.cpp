#include "blinkytapeuploader.h"
#include "blinkypendantuploader.h"
#include "lightbuddyuploader.h"
#include "blinkytape.h"

#include <QDebug>
#include <QTime>


/// Interval between scans to see if the device is still connected
#define CONNECTION_SCANNER_INTERVAL 100

// TODO: Put this somewhere better?
#define PENDANT_MAX_PIXELS 10

#define RESET_TIMER_TIMEOUT 500

#define RESET_MAX_TRIES 3

#define MIN_UPDATE_INTERVAL 3

#define WRITE_BUSY_DELAY 2
#define WRITE_CHUNK_DELAY 4
#define CHUNK_SIZE 300


// TODO: Support a method for loading these from preferences file
QList<QSerialPortInfo> BlinkyTape::probe()
{
    QList<QSerialPortInfo> serialPorts = QSerialPortInfo::availablePorts();
    QList<QSerialPortInfo> tapes;

    foreach (const QSerialPortInfo &info, serialPorts) {
        // Only connect to known BlinkyTapes
        if(info.vendorIdentifier() == BLINKYTAPE_SKETCH_VID
           && info.productIdentifier() == BLINKYTAPE_SKETCH_PID) {
            tapes.push_back(info);
        }
        // If it's a leonardo, it /may/ be a BlinkyTape running a user sketch
        else if(info.vendorIdentifier() == LEONARDO_SKETCH_VID
                && info.productIdentifier() == LEONARDO_SKETCH_PID) {
                 tapes.push_back(info);
        }
        // Also BlinkyPendants!
        else if(info.vendorIdentifier() == BLINKYPENDANT_SKETCH_VID
                && info.productIdentifier() == BLINKYPENDANT_SKETCH_PID) {
                 tapes.push_back(info);
        }
        // And Lightbuddies!
        else if(info.vendorIdentifier() == LIGHTBUDDY_SKETCH_VID
                && info.productIdentifier() == LIGHTBUDDY_SKETCH_PID) {
                 tapes.push_back(info);
        }
    }

    return tapes;
}

// TODO: Support a method for loading these from preferences file
QList<QSerialPortInfo> BlinkyTape::probeBootloaders()
{
    QList<QSerialPortInfo> serialPorts = QSerialPortInfo::availablePorts();
    QList<QSerialPortInfo> tapes;

    foreach (const QSerialPortInfo &info, serialPorts) {
        // Only connect to known BlinkyTapes
        if(info.vendorIdentifier() == BLINKYTAPE_BOOTLOADER_VID
           && info.productIdentifier() == BLINKYTAPE_BOOTLOADER_PID) {
            tapes.push_back(info);
        }
        // If it's a leonardo, it /may/ be a BlinkyTape running a user sketch
        else if(info.vendorIdentifier() == LEONARDO_BOOTLOADER_VID
                && info.productIdentifier() == LEONARDO_BOOTLOADER_PID) {
                 tapes.push_back(info);
        }
    }

    return tapes;
}


BlinkyTape::BlinkyTape(QObject *parent) :
    BlinkyController(parent)
{
    serial = new QSerialPort(this);
    serial->setSettingsRestoredOnClose(false);

    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)),
            this, SLOT(handleSerialError(QSerialPort::SerialPortError)));

    connect(serial, SIGNAL(readyRead()), this, SLOT(handleSerialReadData()));

    connect(serial, SIGNAL(baudRateChanged(qint32, QSerialPort::Directions)),
            this, SLOT(handleBaudRateChanged(qint32, QSerialPort::Directions)));


    resetTimer.setSingleShot(true);
    connect(&resetTimer, SIGNAL(timeout()), this, SLOT(resetTimer_timeout()));

    serialWriteTimer.setSingleShot(true);
    connect(&serialWriteTimer, SIGNAL(timeout()), this, SLOT(sendChunk()));

    // On Windows, we don't get notified if the tape was disconnected, so we have to check peroidically
#if defined(Q_OS_WIN)
    connectionScannerTimer.setInterval(CONNECTION_SCANNER_INTERVAL);
    connect(&connectionScannerTimer, SIGNAL(timeout()), this, SLOT(connectionScannerTimer_timeout()));
#endif
}

void BlinkyTape::handleSerialError(QSerialPort::SerialPortError error)
{
    // The serial library appears to emit an extraneous SerialPortError
    // when open() is called. Just ignore it.
    if(error == QSerialPort::NoError) {
        return;
    }

    QString errorString = serial->errorString();


    if (error == QSerialPort::ResourceError) {
        qCritical() << "Serial resource error, BlinkyTape unplugged?" << errorString;
    }
    else {
        qCritical() << "Unrecognized serial error:" << errorString;
    }

    close();
}

void BlinkyTape::resetTimer_timeout() {
    if(!isConnected()) {
        return;
    }

    qDebug() << "Hit reset timer";

    if(resetTriesRemaining < 1) {
        qCritical() << "Reset timer maximum tries reached, failed to reset tape";
        return;
    }

    serial->setBaudRate(QSerialPort::Baud1200);

    // setBaudRate() doesn't seem to be reliable if called too quickly after the port
    // is opened. In this case,
    resetTimer.start(RESET_TIMER_TIMEOUT);

    resetTriesRemaining--;
}

void BlinkyTape::sendChunk()
{
    // If the last bit didn't get written yet, try again shortly.
    if(serial->bytesToWrite() > 0) {
        qDebug() << "Busy, trying again shortly";
        serialWriteTimer.start(WRITE_BUSY_DELAY);
        return;
    }

    while(!chunks.isEmpty()) {
        QByteArray chunk = chunks.takeFirst();

        int writeLength = serial->write(chunk);
        if(writeLength != chunk.length()) {
            qCritical() << "Error writing all the data out, expected:" << chunk.length()
                        << ", wrote:" << writeLength;
        }
        serial->flush();

        if(!chunks.isEmpty()) {
            serialWriteTimer.start(WRITE_CHUNK_DELAY);
            return;
        }
    }
}

#if defined(Q_OS_WIN)
void BlinkyTape::connectionScannerTimer_timeout() {
    // If we are already disconnected, disregard.
    if(!isConnected()) {
        connectionScannerTimer.stop();
        return;
    }

    // Check if our serial port is on the list
    QSerialPortInfo currentInfo = QSerialPortInfo(*serial);

    QList<QSerialPortInfo> tapes = probe();
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

bool BlinkyTape::open(QSerialPortInfo info) {
    if(isConnected()) {
        qCritical() << "Already connected to a BlinkyTape";
        return false;
    }

    qDebug() << "Connecting to BlinkyTape on " << info.portName();

#if defined(Q_OS_OSX)
    // Note: This should be info.portName(). Changed here as a workaround for:
    // https://bugreports.qt.io/browse/QTBUG-45127
    serial->setPortName(info.systemLocation());
#else
    serial->setPortName(info.portName());
#endif
    serial->setBaudRate(QSerialPort::Baud115200);

    if( !serial->open(QIODevice::ReadWrite) ) {
        qDebug() << "Could not connect to BlinkyTape. Error: " << serial->error() << serial->errorString();
        return false;
    }

    serialInfo = info;

    resetTriesRemaining = 0;

    emit(connectionStatusChanged(true));

#if defined(Q_OS_WIN)
    // Schedule the connection scanner
    connectionScannerTimer.start();
#endif
    return true;
}

void BlinkyTape::close() {
    if(serial->isOpen()) {
        serial->close();
    }

    resetTriesRemaining = 0;

    emit(connectionStatusChanged(isConnected()));
}

void BlinkyTape::handleSerialReadData()
{
    // Discard any data we get back from the BlinkyTape
    serial->readAll();
}

void BlinkyTape::handleBaudRateChanged(qint32 baudRate, QSerialPort::Directions)
{
    if(baudRate == QSerialPort::Baud115200) {
        qDebug() << "Baud rate updated to 115200!";
    }
    else if(baudRate == QSerialPort::Baud1200 && resetTriesRemaining > 0) {
        qDebug() << "Baud rate updated to 1200bps, closing!";

        resetTimer.stop();
        close();
    }
    else if(baudRate == QSerialPort::Baud1200) {
        qDebug() << "Baud rate updated to 1200bps spuriously";
    }
}

bool BlinkyTape::isConnected() {
    return serial->isOpen();
}

void BlinkyTape::sendUpdate(QByteArray ledData)
{
    // Ignore the update request if it came too quickly
    static qint64 lastTime = 0;
    qint64 newTime = QDateTime::currentMSecsSinceEpoch();
    if (newTime - lastTime < MIN_UPDATE_INTERVAL) {
        return;
    }
    lastTime = newTime;

    if(!isConnected()) {
        qCritical() << "Strip not connected, not sending update!";
        return;
    }

    // If there is data pending to send, skip this update to prevent overflowing
    // the buffer.
    if(!chunks.isEmpty()) {
        qDebug() << "Output data still in buffer, dropping this update frame";
        return;
    }

    // If we have a blinkyPendant, fit the data to the output device
    if(serialInfo.vendorIdentifier() == BLINKYPENDANT_SKETCH_VID
        && serialInfo.productIdentifier() == BLINKYPENDANT_SKETCH_PID) {
        ledData = ledData.leftJustified(PENDANT_MAX_PIXELS*3, (char)0x00, true);
    }

    // Trim anything that's 0xff
    for(int i = 0; i < ledData.length(); i++) {
        if(ledData[i] == (char)255) {
            ledData[i] = 254;
        }
    }

    // Append an 0xFF to signal the flip command
    ledData.append(0xFF);

    for(int chunk = 0; chunk*CHUNK_SIZE < ledData.length(); chunk++) {

        int chunkLength = CHUNK_SIZE;
        if((chunk+1)*CHUNK_SIZE > ledData.length()) {
           chunkLength = ledData.length() - chunk*CHUNK_SIZE;
        }

        QByteArray chunkData = ledData.mid(chunk*CHUNK_SIZE,chunkLength);
        chunks.append(chunkData);

//        qDebug() << "size=" << ledData.length() << " chunk=" << chunk << " position=" << chunk*CHUNK_SIZE << " length=" << chunkLength;
//        qDebug() << chunkData.toHex();
    }

//    qDebug() << "Total chunks:" << chunks.size();
    sendChunk();
}

bool BlinkyTape::getPortInfo(QSerialPortInfo& info)
{
    if(!isConnected()) {
        return false;
    }

    info = serialInfo;
    return true;
}

void BlinkyTape::reset()
{
    if(!isConnected()) {
        return;
    }

    qDebug() << "Attempting to reset BlinkyTape";

    resetTriesRemaining = RESET_MAX_TRIES;
    resetTimer_timeout();
}


bool BlinkyTape::getUploader(QPointer<PatternUploader>& uploader)
{
    if(!isConnected()) {
        return false;
    }

    if(serialInfo.vendorIdentifier() == BLINKYTAPE_SKETCH_VID
        && serialInfo.productIdentifier() == BLINKYTAPE_SKETCH_PID) {
        uploader = new BlinkyTapeUploader(parent());
    }
    else if(serialInfo.vendorIdentifier() == LEONARDO_SKETCH_VID
        && serialInfo.productIdentifier() == LEONARDO_SKETCH_PID) {
        uploader = new BlinkyTapeUploader(parent());
    }
    else if(serialInfo.vendorIdentifier() == BLINKYPENDANT_SKETCH_VID
        && serialInfo.productIdentifier() == BLINKYPENDANT_SKETCH_PID) {
        uploader = new BlinkyPendantUploader(parent());
    }
    else if(serialInfo.vendorIdentifier() == LIGHTBUDDY_SKETCH_VID
        && serialInfo.productIdentifier() == LIGHTBUDDY_SKETCH_PID) {
        uploader = new LightBuddyUploader(parent());
    }
    else {
        return false;
    }

    return true;
}
