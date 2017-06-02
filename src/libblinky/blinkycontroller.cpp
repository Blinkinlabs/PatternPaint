#include "blinkycontroller.h"
#include "controllerinfo.h"
#include "blinkytapecontrollerinfo.h"
#include "usbutils.h"
#include "usbdeviceidentifier.h"

#include <QList>
#include <QSerialPortInfo>
#include <QPointer>
#include <QDebug>

BlinkyController::BlinkyController(QObject *parent) : QObject(parent)
{
}

QList<QPointer<ControllerInfo> > BlinkyController::probe()
{
    QList<QSerialPortInfo> serialPorts = usbUtils::getUsefulSerialPorts();
    QList<QPointer<ControllerInfo> > controllerInfos;

    for (const QSerialPortInfo &info : serialPorts) {
        for(UsbDeviceIdentifier identifier : blinkyControllers) {
            if(identifier.matches(info))
                controllerInfos.push_back(new BlinkyTapeControllerInfo(info));
        }
    }

    return controllerInfos;
}

QList<QSerialPortInfo> BlinkyController::probeBootloaders()
{
    QList<QSerialPortInfo> serialPorts = usbUtils::getUsefulSerialPorts();
    QList<QSerialPortInfo> bootloaders;

    for (const QSerialPortInfo &info : serialPorts) {
        for(UsbDeviceIdentifier identifier : firmwareLoaders) {
            if(identifier.matches(info))
                bootloaders.push_back(info);
        }
    }

    return bootloaders;
}
