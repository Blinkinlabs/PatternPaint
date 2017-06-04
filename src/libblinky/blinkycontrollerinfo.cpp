#include "blinkycontrollerinfo.h"
#include "usbutils.h"
#include "usbdeviceidentifier.h"

#include <QSerialPortInfo>

QList<BlinkyControllerInfo> BlinkyControllerInfo::availableControllers()
{
    QList<QSerialPortInfo> serialPorts = usbUtils::getUsefulSerialPorts();
    QList<BlinkyControllerInfo> controllerInfos;

    for (const QSerialPortInfo &info : serialPorts) {
        for(UsbDeviceIdentifier identifier : blinkyControllers) {
            if(identifier.matches(info))
                controllerInfos.push_back(BlinkyControllerInfo(info));
        }
    }

    return controllerInfos;
}

BlinkyControllerInfo::BlinkyControllerInfo(const QSerialPortInfo &info) :
    info(info)
{

}

QString BlinkyControllerInfo::resourceName() const
{
    return info.portName();
}


const QSerialPortInfo &BlinkyControllerInfo::getInfo() const
{
    return info;
}
