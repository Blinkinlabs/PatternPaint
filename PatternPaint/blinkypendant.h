#ifndef BLINKYPENDANT_H
#define BLINKYPENDANT_H

#include <QObject>
#include <QList>
#include <QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "blinkycontroller.h"

#define BLINKYPENDANT_SKETCH_VID           0x1209
#define BLINKYPENDANT_SKETCH_PID           0x8888
#define BLINKYPENDANT_BOOTLOADER_VID       0x1209
#define BLINKYPENDANT_BOOTLOADER_PID       0x8889

//#define FLASH_MEMORY_AVAILABLE          0x7000  // Amount of application space in the flash
//#define FLASH_MEMORY_PAGE_SIZE          0x80    // Size of a page of memory in our flash
//#define FLASH_MEMORY_SKETCH_ADDRESS     0x0000  // Location of sketch
//#define FLASH_MEMORY_PATTERN_TABLE_ADDRESS (FLASH_MEMORY_AVAILABLE - FLASH_MEMORY_PAGE_SIZE) // Location of pattern table

/// Connect to a BlinkyPendant over a serial port, and manage sending data to it.
class BlinkyPendant : public BlinkyController
{
    Q_OBJECT
public:
    static QList<QSerialPortInfo> probe();
    static QList<QSerialPortInfo> probeBootloaders();

    BlinkyPendant(QObject *parent);

    bool isConnected();
    bool open(QSerialPortInfo info);
    void sendUpdate(QByteArray colors);
    bool getPortInfo(QSerialPortInfo &info);

    bool getUploader(QPointer<PatternUploader>& uploader);

    // Atempt to reset the strip by setting it's baud rate to 1200 and closing it.
    void reset();

public slots:

    void close();

private:
    /// Serial port the BlinkyPendant is connected to
    QPointer<QSerialPort> serial;

#if defined(Q_OS_WIN)
    // Windows only: Timer that periodically checks if the serial device is
    // still present (it seems to disappear without sending an error?)
    // Handle this by periodically scanning for available BlinkyPendants, and
    // close this one if it no longer in the list
    // TODO: Pull the latest QtSerialPort, it seems to have a fix for this
    QTimer *connectionScannerTimer;
#endif

signals:
    void connectionStatusChanged(bool status);

private slots:
    void handleSerialError(QSerialPort::SerialPortError error);

    void handleSerialReadData();

#if defined(Q_OS_WIN)
    void connectionScannerTimer_timeout();
#endif
};

#endif // BLINKYPENDANT_H
