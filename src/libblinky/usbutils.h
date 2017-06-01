#ifndef USBUTILS
#define USBUTILS

#include <QList>
#include <QSerialPortInfo>

// Some probe commands for USB devices, that are probably best integrated somewhere else.

namespace usbUtils {

QList<QSerialPortInfo> getUsefulSerialPorts();

int getVersionForDevice(quint16 idVendor, quint16 idProduct);

}

#endif // USBUTILS
