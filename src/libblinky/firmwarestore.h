#ifndef FIRMWARESTORE_H
#define FIRMWARESTORE_H

#include <QString>

#include "memorysection.h"

#define FIRMWARE_FOLDER "/Pattern Paint/Firmware/" /// Default folder for Firmware import
#define FIRMWARE_DESCRIPTION_FILE "README.md"

// TODO: Put these somewhere specific to the BlinkyTape
#define BLINKYTAPE_FACTORY_FIRMWARE_NAME "factory"
#define BLINKYTAPE_DEFAULT_FIRMWARE_NAME "default"

class FirmwareStore
{
public:
    static QStringList listFirmwareSearchPaths();
    static QStringList listAvailableFirmware();

    static QString getFirmwareDirectoryName(const QString &name);
    static QStringList getFirmwareDescription(const QString &name);
    static MemorySection getFirmwareData(const QString &name);

    bool addFirmware(const QString &sourcePathName);
    bool removeFirmware(const QString &name);

    QString getErrorString() const;

private:
    QString errorString;
};

#endif // FIRMWARESTORE_H
