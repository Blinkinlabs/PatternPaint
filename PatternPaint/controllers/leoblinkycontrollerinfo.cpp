#include "leoblinkycontrollerinfo.h"
#include "leoblinky.h"

LeoBlinkyControllerInfo::LeoBlinkyControllerInfo(QSerialPortInfo info, QObject *parent) :
    ControllerInfo(parent),
    info(info)
{
}

QString LeoBlinkyControllerInfo::resourceName() const
{
    return info.portName();
}

BlinkyController *LeoBlinkyControllerInfo::createController() const
{
    BlinkyController *controller = new LeoBlinky(info, NULL);

    return controller;
}
