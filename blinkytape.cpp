#include "avrprogrammer.h"
#include "blinkytape.h"
#include <QDebug>

#include <cmath>

// TODO: Support a method for loading these from preferences file
QList<QSerialPortInfo> BlinkyTape::findBlinkyTapes()
{
    QList<QSerialPortInfo> tapes;

    // TODO: Should we use VID/PID here instead of descriptions?
    // Maybe it's not the most important thing in the world.
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        // Only connect to know BlinkyTapes
        if(info.vendorIdentifier() == BLINKYTAPE_SKETCH_VID
           && info.productIdentifier() == BLINKYTAPE_SKETCH_PID) {
            tapes.push_back(info);
        }
        // If it's a leonardo, it /may/ be a BlinkyTape running a user sketch
        else if(info.vendorIdentifier() == LEONARDO_SKETCH_VID
                && info.productIdentifier() == LEONARDO_SKETCH_PID) {
                 tapes.push_back(info);
        }
    }

    return tapes;
}

// TODO: Support a method for loading these from preferences file
QList<QSerialPortInfo> BlinkyTape::findBlinkyTapeBootloaders()
{
    QList<QSerialPortInfo> tapes;
    // TODO: Should we use VID/PID here instead of descriptions?
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        qDebug() << info.productIdentifier();
        qDebug() << info.vendorIdentifier();

        // Only connect to know BlinkyTapes
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

QRgb BlinkyTape::correctBrightness(QRgb uncorrected)
{
    return qRgb(
        int(255*pow(qRed(  uncorrected)/255.0, 1.8)),
        int(255*pow(qGreen(uncorrected)/255.0, 1.8)),
        int(255*pow(qBlue( uncorrected)/255.0, 2.1))
        );
}

BlinkyTape::BlinkyTape(QObject *parent, int ledCount_) :
    QObject(parent)
{
    ledCount = ledCount_;

    serial = NULL;
}

void BlinkyTape::handleSerialError(QSerialPort::SerialPortError error)
{
    if(error == QSerialPort::NoError) {
        // The serial library appears to emit an extraneous SerialPortError
        // when open() is called. Just ignore it.
        return;
    }

    if(serial == NULL) {
        qCritical() << "Got error after serial device was deleted!";
        return;
    }

    // TODO: handle other error types?
    if (error == QSerialPort::ResourceError) {
        qCritical() << serial->errorString();
        close();
    }
}

bool BlinkyTape::open(QSerialPortInfo info) {
    if(serial == NULL) {
        serial = new QSerialPort(this);
        connect(serial, SIGNAL(error(QSerialPort::SerialPortError)),
                this, SLOT(handleSerialError(QSerialPort::SerialPortError)));
    }

    if(isConnected()) {
        qCritical() << "Already connected to a BlinkyTape";
        return false;
    }

    qDebug() << "Connecting to BlinkyTape on " << info.portName();
    serial->setPort(info);
    serial->open(QIODevice::ReadWrite);

    if(!isConnected()) {
        qDebug() << "Could not connect to BlinkyTape";
        return false;
    }

    emit(connectionStatusChanged(true));
    return true;
}

void BlinkyTape::close() {
    if(serial == NULL) {
        return;
    }

    serial->close();
    serial->deleteLater();
    serial = NULL;

    emit(connectionStatusChanged(isConnected()));
}

bool BlinkyTape::isConnected() {
    if(serial == NULL) {
        return false;
    }

    return serial->isOpen();
}

void BlinkyTape::sendUpdate(QByteArray LedData)
{
    if(!isConnected()) {
        // TODO: Signal error?
        qCritical() << "Strip not connected, not sending update!";
        return;
    }

    // TODO: Check if we can write to the device?

    if(LedData.length() != ledCount*3) {
        qCritical() << "Length not correct, not sending update!";
        return;
    }

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

bool BlinkyTape::getPortInfo(QSerialPortInfo& info)
{
    if(!isConnected()) {
        return false;
    }

    info = QSerialPortInfo(*serial);
    return true;
}

void BlinkyTape::reset()
{
    if(!isConnected()) {
        return;
    }

    qDebug() << "Attempting to reset BlinkyTape";
    serial->setSettingsRestoredOnClose(false);
    serial->setBaudRate(QSerialPort::Baud1200);

    close();
}
