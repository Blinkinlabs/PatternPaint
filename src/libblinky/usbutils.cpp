#include "usbutils.h"

#include <QList>
#include <QSerialPortInfo>

#include <QDebug>
#include <libusb-1.0/libusb.h>

QList<QSerialPortInfo> getUsefulSerialPorts()
{
    QList<QSerialPortInfo> usablePorts;

    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {

#if defined(Q_OS_MACX)
        if(info.portName().startsWith("cu"))
            continue;
        if(info.portName().endsWith("Bluetooth-Incoming-Port"))
            continue;
#endif

        usablePorts.append(info);
    }

    return usablePorts;
}

// TODO: This returns the first matching device. Should allow side channel info to look
// up an exact match.
int getVersionForDevice(quint16 idVendor, quint16 idProduct)
{
    libusb_device **devs;
    int r;
    ssize_t cnt;

    r = libusb_init(NULL);

    if (r < 0)
        return -1;

    cnt = libusb_get_device_list(NULL, &devs);
    if (cnt < 0)
        return -2;

    int version = -3;

    libusb_device *dev;
    int i = 0;

    while ((dev = devs[i++]) != NULL) {
        struct libusb_device_descriptor desc;
        int r = libusb_get_device_descriptor(dev, &desc);
        if (r < 0) {
            qDebug() << "failed to get device descriptor";
            break;
        }
        if (desc.idVendor == idVendor && desc.idProduct == idProduct) {
            version = desc.bcdDevice;
            break;
        }
    }

    libusb_free_device_list(devs, 1);
    return version;
}
