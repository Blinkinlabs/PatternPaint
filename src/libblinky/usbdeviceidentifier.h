#ifndef USBDEVICEIDENTIFIER_H
#define USBDEVICEIDENTIFIER_H

#include <QSerialPortInfo>
#include <cstdint>

class UsbDeviceIdentifier {
public:
    uint16_t vid;
    uint16_t pid;
    QString name;

    bool matches(const QSerialPortInfo &info) const;
};

// List of USB devices that we can associate with a BlinkyController
extern UsbDeviceIdentifier blinkyControllers[6];

// List of USB devices that we can associate with a PatternUploader
extern UsbDeviceIdentifier patternUploaders[6];

// List of USB devices that we can associate with a FirmwareLoader
extern UsbDeviceIdentifier firmwareLoaders[6];

#endif // USBDEVICEIDENTIFIER_H
