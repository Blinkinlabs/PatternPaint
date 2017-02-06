#ifndef BLINKYCONTROLLER_H
#define BLINKYCONTROLLER_H

#include "controllerinfo.h"

#include <QObject>
#include <QList>

#define BLINKYTAPE_SKETCH_VID           0x1D50
#define BLINKYTAPE_SKETCH_PID           0x605E
#define BLINKYTAPE_BOOTLOADER_VID       0x1D50
#define BLINKYTAPE_BOOTLOADER_PID       0x606C

#define LEONARDO_SKETCH_VID             0x2341
#define LEONARDO_SKETCH_PID             0x8036
#define LEONARDO_BOOTLOADER_VID         0x2341
#define LEONARDO_BOOTLOADER_PID         0x0036

#define ARDUINOMICRO_SKETCH_VID         0x2341
#define ARDUINOMICRO_SKETCH_PID         0x8037
#define ARDUINOMICRO_BOOTLOADER_VID     0x2341
#define ARDUINOMICRO_BOOTLOADER_PID     0x0037

#define BLINKYPENDANT_SKETCH_VID        0x1209
#define BLINKYPENDANT_SKETCH_PID        0x8888
#define BLINKYPENDANT_BOOTLOADER_VID    0x1209
#define BLINKYPENDANT_BOOTLOADER_PID    0x8889

#define LIGHTBUDDY_SKETCH_VID        0x1d50
#define LIGHTBUDDY_SKETCH_PID        0x60aa
#define LIGHTBUDDY_BOOTLOADER_VID    0x1d50
#define LIGHTBUDDY_BOOTLOADER_PID    0x60a9


#define EIGHTBYEIGHT_SKETCH_VID        0x1d50
#define EIGHTBYEIGHT_SKETCH_PID        0x60f6
#define EIGHTBYEIGHT_BOOTLOADER_VID    0x1d50
#define EIGHTBYEIGHT_BOOTLOADER_PID    0x60f5

class BlinkyUploader;
class QSerialPortInfo;

/// Connect to an LED controller, and manage sending data to it.
class BlinkyController : public QObject
{
    Q_OBJECT
public:
    static QList<QPointer<ControllerInfo> > probe();
    static QList<QSerialPortInfo> probeBootloaders();

    BlinkyController(QObject *parent);

    virtual QString getName() const = 0;

    virtual bool isConnected() = 0;

    virtual bool open() = 0;

    virtual void sendUpdate(QByteArray colors) = 0;

    virtual bool getPortInfo(QSerialPortInfo &info) = 0;

    virtual bool getUploader(QPointer<BlinkyUploader> &uploader) = 0;

    // Reset the controller
    virtual void reset() = 0;

public slots:

    virtual void close();
};

#endif // BLINKYCONTROLLER_H
