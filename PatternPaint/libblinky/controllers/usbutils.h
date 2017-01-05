#ifndef USBUTILS
#define USBUTILS

#endif // USBUTILS

#include <QtGlobal>

// Some probe commands for USB devices, that are probably best integrated somewhere else.

int getVersionForDevice(quint16 idVendor, quint16 idProduct);
