#ifndef FIRMWARESTORETESTS_H
#define FIRMWARESTORETESTS_H

#include <QObject>

class FirmwareStoreTests : public QObject
{
    Q_OBJECT
private slots:
    void listAvailableFirmwareHasDefaultTest();
    void getFirmwareDescriptionHasDefaultTest();

    void addFirmwareBadSourceDirectoryTest();
    void addFirmwareEmptyDirectoryTest();

    void removeFirmwareDefaultFails();
};

#endif // FIRMWARESTORETESTS_H
