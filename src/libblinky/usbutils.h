#ifndef USBUTILS
#define USBUTILS

#include "libblinkyglobal.h"

#include <QList>
#include <QSerialPortInfo>

// Some probe commands for USB devices, that are probably best integrated somewhere else.

namespace usbUtils {

LIBBLINKY_EXPORT QList<QSerialPortInfo> getUsefulSerialPorts();

LIBBLINKY_EXPORT int getVersionForDevice(quint16 idVendor, quint16 idProduct);

}

#endif // USBUTILS
