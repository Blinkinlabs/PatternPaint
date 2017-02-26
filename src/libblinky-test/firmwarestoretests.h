#ifndef FIRMWARESTORETESTS_H
#define FIRMWARESTORETESTS_H

#include <QObject>

class FirmwareStoreTests : public QObject
{
    Q_OBJECT
private slots:    
    void listFirmwareSearchPathsTest();

    void addFirmwareBadSourceDirectoryTest();
    void addFirmwareEmptyDirectoryTest();
    void addFirmwareAlreadyExistsTest();

    void removeFirmwareDoesntExitTest();
    void removeFirmwareDefaultFailsTest();

    void listAvailableFirmwareBlinkyTapeFirmwaresTest_data();
    void listAvailableFirmwareBlinkyTapeFirmwaresTest();

    void getFirmwareDirectoryNameDoesntExistTest();
    void getFirmwareDirectoryNameHasBlinkyTapeDefaultTest();

    void getFirmwareDescriptionDoesntExistTest();
    void getFirmwareDescriptionHasBlinkyTapeDefaultTest();

    void getFirmwareDataDoesntExistTest();
    void getFirmwareDataHasBlinkyTapeDefaultTest();
};

#endif // FIRMWARESTORETESTS_H
