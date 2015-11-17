#ifndef BLINKYCONTROLLER_H
#define BLINKYCONTROLLER_H

#include <QObject>
#include <QList>

#define LIGHT_BUDDY_APPLICATION_VID     0x1D50
#define LIGHT_BUDDY_APPLICATION_PID     0x60AA
#define LIGHT_BUDDY_BOOTLOADER_VID      0x1D50
#define LIGHT_BUDDY_BOOTLOADER_PID      0x60A9

class PatternUploader;
class QSerialPortInfo;

/// Connect to a BlinkyTape over a serial port, and manage sending data to it.
class BlinkyController : public QObject
{
    Q_OBJECT
public:
    BlinkyController(QObject *parent);

    virtual bool isConnected() = 0;

    virtual bool open(QSerialPortInfo info) = 0;

    virtual void sendUpdate(QByteArray colors) = 0;

    virtual bool getPortInfo(QSerialPortInfo &info) = 0;

    virtual bool getUploader(QPointer<PatternUploader>& uploader) = 0;

    // Atempt to reset the strip by setting it's baud rate to 1200 and closing it.
    virtual void reset() = 0;

public slots:

    virtual void close();
};

#endif // BLINKYCONTROLLER_H
