#ifndef FIRMWAREIMPORT_H
#define FIRMWAREIMPORT_H

#include <QString>

#define FIRMWARE_FOLDER "/Pattern Paint/Firmware/" /// Default folder for Firmware import
#define DEFAULT_FIRMWARE_NAME "default"

extern QByteArray FIRMWARE_DATA;
extern QString FIRMWARE_NAME;

class firmwareimport
{
public:
    bool firmwareRead(const QString filename);
};

#endif // FIRMWAREIMPORT_H
