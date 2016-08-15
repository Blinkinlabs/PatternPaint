#ifndef LEOBLINKYCONTROLLERINFO_H
#define LEOBLINKYCONTROLLERINFO_H

#include "controllerinfo.h"

#include <QSerialPortInfo>
#include <QObject>

class LeoBlinkyControllerInfo : public ControllerInfo
{
public:
    LeoBlinkyControllerInfo(QSerialPortInfo info, QObject *parent = 0);

    QString resourceName() const;

    BlinkyController *createController() const;

private:
    QSerialPortInfo info;
};

#endif // LEOBLINKYCONTROLLERINFO_H
