#ifndef BLINKYCONTROLLER_H
#define BLINKYCONTROLLER_H

#include "controllerinfo.h"

#include <cstdint>
#include <QObject>
#include <QList>

#include "firmwareloader.h"
#include "blinkyuploader.h"

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

    virtual bool getPatternUploader(QPointer<BlinkyUploader> &uploader) = 0;

    virtual bool getFirmwareLoader(QPointer<FirmwareLoader> &loader) = 0;

    // Reset the controller
    virtual void reset() = 0;

public slots:

    virtual void close() = 0;

signals:
    virtual void connectionStatusChanged(bool status);
};

#endif // BLINKYCONTROLLER_H
