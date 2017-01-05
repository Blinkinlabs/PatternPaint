#ifndef BLINKYTAPECONTROLLERINFO_H
#define BLINKYTAPECONTROLLERINFO_H

#include "controllerinfo.h"

#include <QSerialPortInfo>
#include <QObject>

class BlinkyTapeControllerInfo : public ControllerInfo
{
    Q_OBJECT

public:
    BlinkyTapeControllerInfo(QSerialPortInfo info, QObject *parent = 0);

    QString resourceName() const;

    BlinkyController *createController() const;

private:
    QSerialPortInfo info;
};

#endif // BLINKYTAPECONTROLLERINFO_H
