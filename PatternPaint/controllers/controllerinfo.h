#ifndef CONTROLLERINFO_H
#define CONTROLLERINFO_H

#include <QString>
#include <QObject>

class BlinkyController;

class ControllerInfo : public QObject
{
Q_OBJECT

public:
    ControllerInfo(QObject *parent = 0) : QObject(parent) {}
    virtual ~ControllerInfo() {}

    /// Get the resource name used to locate this device
    /// For example: Serial port location
    virtual QString resourceName() const = 0;

    /// Attempt to create a controller based on this resource locater
    virtual BlinkyController* createController() const = 0;
};

#endif // CONTROLLERINFO_H
