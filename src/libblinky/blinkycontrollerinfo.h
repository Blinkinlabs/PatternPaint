#ifndef CONTROLLERINFO_H
#define CONTROLLERINFO_H

#include <QString>
#include <QObject>
#include <QList>

#include "blinkycontroller.h"

class BlinkyControllerInfo
{
public:
    static QList<BlinkyControllerInfo> availableControllers();

    BlinkyControllerInfo(const QSerialPortInfo &info);

    /// Get the resource name used to locate this device
    /// For example: Serial port location
    QString resourceName() const;

    const QSerialPortInfo &getInfo() const;

private:

    // TODO: push this to a private stucture
    QSerialPortInfo info;
};

#endif // CONTROLLERINFO_H
