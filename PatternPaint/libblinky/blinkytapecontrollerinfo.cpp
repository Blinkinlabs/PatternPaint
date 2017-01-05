#include "blinkytapecontrollerinfo.h"

#include "blinkycontroller.h"
#include "blinkytape.h"

BlinkyTapeControllerInfo::BlinkyTapeControllerInfo(QSerialPortInfo info, QObject *parent) :
    ControllerInfo(parent),
    info(info)
{
}

QString BlinkyTapeControllerInfo::resourceName() const
{
    return info.portName();
}

BlinkyController *BlinkyTapeControllerInfo::createController() const
{
    BlinkyController *controller = new BlinkyTape(info, NULL);

    return controller;
}
