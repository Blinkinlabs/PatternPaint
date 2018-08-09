#ifndef USBDEVICEIDENTIFIER_H
#define USBDEVICEIDENTIFIER_H

#include "libblinkyglobal.h"

#include <QSerialPortInfo>
#include <cstdint>

class LIBBLINKY_EXPORT UsbDeviceIdentifier {
public:
    uint16_t vid;
    uint16_t pid;
    QString name;

    bool matches(const QSerialPortInfo &info) const;
};

// List of USB devices that we can associate with a BlinkyController
extern UsbDeviceIdentifier blinkyControllers[7];

// List of USB devices that we can associate with a PatternUploader
extern UsbDeviceIdentifier patternUploaders[7];

// List of USB devices that we can associate with a FirmwareLoader
extern UsbDeviceIdentifier firmwareLoaders[6];

#endif // USBDEVICEIDENTIFIER_H
