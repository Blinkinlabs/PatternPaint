#ifndef FIRMWARESTORE_H
#define FIRMWARESTORE_H


#include <QString>

#define FIRMWARE_FOLDER "/Pattern Paint/Firmware/" /// Default folder for Firmware import
#define DEFAULT_FIRMWARE_NAME "Default"
#define FIRMWARE_DESCRIPTION_FILE "README.md"

class FirmwareStore
{
public:
    static QStringList listAvailableFirmware();
    static QString getFirmwareDescription(const QString &name);

    bool addFirmware(const QString &sourcePathName);
    bool removeFirmware(const QString &name);

    QString getErrorString() const;

private:
    QString errorString;
};

#endif // FIRMWARESTORE_H
