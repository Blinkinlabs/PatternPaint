#ifndef BLINKYCONTROLLER_H
#define BLINKYCONTROLLER_H

#include <cstdint>
#include <QObject>
#include <QList>
#include <QSerialPortInfo>
#include <QPointer>

#include "firmwareloader.h"
#include "blinkyuploader.h"

class BlinkyControllerInfo;

/// Connect to an LED controller, and manage sending data to it.
class BlinkyController : public QObject
{
    Q_OBJECT
public:
    static BlinkyController *create(const BlinkyControllerInfo &info, QObject *parent);

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
    void connectionStatusChanged(bool status);
};

#endif // BLINKYCONTROLLER_H
