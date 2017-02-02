#ifndef FIRMWAREIMPORT_H
#define FIRMWAREIMPORT_H

#include <QString>

#define FIRMWARE_FOLDER "/Pattern Paint/Firmware/" /// Default folder for Firmware import
#define DEFAULT_FIRMWARE_NAME "Default"
#define FIRMWARE_DESCRIPTION_FILE "README.md"

class FirmwareStore
{
public:
    static QStringList listAvailableFirmware();
    static QString getFirmwareDescription(const QString &name);

    bool addFirmware(const QString &dirSource);
    bool removeFirmware(const QString &name);

    QString getErrorString() const;

private:
    QString errorString;
};

class FirmwareReader
{
public:
    bool load(const QString &filename);

    const QByteArray &getData() const;
    const QString &getName() const;

private:
    QByteArray data;
    QString name;
};

#endif // FIRMWAREIMPORT_H
