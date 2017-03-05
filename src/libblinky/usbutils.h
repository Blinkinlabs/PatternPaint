#ifndef USBUTILS
#define USBUTILS

#endif // USBUTILS

#include <QList>
#include <QSerialPortInfo>

// Some probe commands for USB devices, that are probably best integrated somewhere else.

QList<QSerialPortInfo> getUsefulSerialPorts();

int getVersionForDevice(quint16 idVendor, quint16 idProduct);
