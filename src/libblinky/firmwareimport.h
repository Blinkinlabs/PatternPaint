#ifndef FIRMWAREIMPORT_H
#define FIRMWAREIMPORT_H

#include <QString>

#define FIRMWARE_FOLDER "/Pattern Paint/Firmware/" /// Default folder for Firmware import
#define DEFAULT_FIRMWARE_NAME "Default"
#define FIRMWARE_DESCRIPTION_FILE "README.md"

extern QString errorStringFirmware;

class firmwareimport
{
public:
    static QStringList listAvailableFirmware();
    static QString getFirmwareDescription(const QString &name);

    static bool removeFirmware(const QString &name);
    static bool addFirmware(const QString &dirSource);

public:
    bool firmwareRead(const QString &filename);

    const QByteArray &getData() const;
    const QString &getName() const;

private:
    QByteArray data;
    QString name;
};

#endif // FIRMWAREIMPORT_H
