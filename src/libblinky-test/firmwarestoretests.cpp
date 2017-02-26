#include "firmwarestoretests.h"
#include "firmwarestore.h"

#include <QtTest>

void FirmwareStoreTests::listAvailableFirmwareHasDefaultTest()
{
    QStringList availableFirmware = FirmwareStore::listAvailableFirmware();

    QVERIFY(availableFirmware.contains(DEFAULT_FIRMWARE_NAME));
}

void FirmwareStoreTests::getFirmwareDescriptionHasDefaultTest()
{
    QString description = FirmwareStore::getFirmwareDescription(DEFAULT_FIRMWARE_NAME);

    QString expectedDescription;
    expectedDescription.append("Default BlinkyTape Firmware\n");
    expectedDescription.append("Use this for all standard functions");

    QVERIFY(description == expectedDescription);
}

void FirmwareStoreTests::addFirmwareBadSourceDirectoryTest()
{
    FirmwareStore firmwareStore;
    QString expectedError = "Source directory doesn't exist";

    QVERIFY(firmwareStore.addFirmware("qrc:///does/not/exist") == false);
    QVERIFY(firmwareStore.getErrorString() == expectedError);
}

void FirmwareStoreTests::addFirmwareEmptyDirectoryTest()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    FirmwareStore firmwareStore;
    QString expectedError = "Firmware hex file not found";

    QVERIFY(firmwareStore.addFirmware(dir.path()) == false);
    QVERIFY(firmwareStore.getErrorString() == expectedError);
}

void FirmwareStoreTests::removeFirmwareDefaultFails()
{
    FirmwareStore firmwareStore;
    QString expectedError = "Cannot remove default firmware";

    QVERIFY(firmwareStore.removeFirmware(DEFAULT_FIRMWARE_NAME) == false);
    QVERIFY(firmwareStore.getErrorString() == expectedError);
}
