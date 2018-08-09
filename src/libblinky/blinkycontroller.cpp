#include "blinkycontroller.h"
#include "blinkycontrollerinfo.h"
#include "usbutils.h"
#include "usbdeviceidentifier.h"

#include <QList>
#include <QDebug>

#include "blinkytape.h"
#include "leoblinky.h"

BlinkyController *BlinkyController::create(const BlinkyControllerInfo &info, QObject *parent)
{
    // TODO: Why is this not being set based on the descriptor list???

    return new LeoBlinky(info.getInfo(), parent);

//    return new BlinkyTape(info.getInfo(), parent);
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

BlinkyController::BlinkyController(QObject *parent) : QObject(parent)
{
}
