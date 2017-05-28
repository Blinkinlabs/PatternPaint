#include "firmwarestoretests.h"
#include "firmwarestore.h"

#include <QtTest>

void FirmwareStoreTests::listFirmwareSearchPathsTest()
{
    // There should be at least the BlinkyTape built-in and third-party locations
    QCOMPARE(FirmwareStore::listFirmwareSearchPaths().count(), 2);
}

void FirmwareStoreTests::addFirmwareBadSourceDirectoryTest()
{
    FirmwareStore firmwareStore;
    QString expectedError = "Source directory doesn't exist";

    QCOMPARE(firmwareStore.addFirmware(":/does/not/exist"), false);
    QCOMPARE(firmwareStore.getErrorString(), expectedError);
}

void FirmwareStoreTests::addFirmwareEmptyDirectoryTest()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    FirmwareStore firmwareStore;
    QString expectedError = "Firmware hex file not found";

    QCOMPARE(firmwareStore.addFirmware(dir.path()), false);
    QCOMPARE(firmwareStore.getErrorString(), expectedError);
}

void FirmwareStoreTests::addFirmwareAlreadyExistsTest()
{
    FirmwareStore firmwareStore;
    QString expectedError = "Firmware with this name already exists, please use a different name!";

    QCOMPARE(firmwareStore.addFirmware(BLINKYTAPE_DEFAULT_FIRMWARE_NAME), false);
    QCOMPARE(firmwareStore.getErrorString(), expectedError);
}

void FirmwareStoreTests::removeFirmwareDoesntExitTest()
{
    FirmwareStore firmwareStore;
    QString expectedError = "No firmware with that name found";

    QCOMPARE(firmwareStore.removeFirmware("thisfirmwaredoesntexist"), false);
    QCOMPARE(firmwareStore.getErrorString(), expectedError);
}

void FirmwareStoreTests::removeFirmwareDefaultFailsTest()
{
    FirmwareStore firmwareStore;
    QString expectedError = "Cannot remove built-in firmware";

    QCOMPARE(firmwareStore.removeFirmware(BLINKYTAPE_DEFAULT_FIRMWARE_NAME), false);
    QCOMPARE(firmwareStore.getErrorString(), expectedError);
}

void FirmwareStoreTests::listAvailableFirmwareBlinkyTapeFirmwaresTest_data()
{
    // Test that we have all of the firmware needed for BlinkyTape functionality

    QTest::addColumn<QString>("name");

    QTest::newRow("defaut") << QString("default");
    QTest::newRow("factory") << QString("factory");
}

void FirmwareStoreTests::listAvailableFirmwareBlinkyTapeFirmwaresTest()
{
    QFETCH(QString, name);

    QStringList availableFirmware = FirmwareStore::listAvailableFirmware();

    QVERIFY(availableFirmware.contains(name));
}

void FirmwareStoreTests::getFirmwareDirectoryNameDoesntExistTest()
{
    QVERIFY(FirmwareStore::getFirmwareDirectoryName("thisfirmwaredoesntexist").isNull());
}

void FirmwareStoreTests::getFirmwareDirectoryNameHasBlinkyTapeDefaultTest()
{
    QString expectedDirectory(":/firmware/blinkytape/default");

    QCOMPARE(FirmwareStore::getFirmwareDirectoryName(BLINKYTAPE_DEFAULT_FIRMWARE_NAME), expectedDirectory);
}

void FirmwareStoreTests::getFirmwareDescriptionDoesntExistTest()
{
    QVERIFY(FirmwareStore::getFirmwareDescription("thisfirmwaredoesntexist").isEmpty());
}


void FirmwareStoreTests::getFirmwareDescriptionHasBlinkyTapeDefaultTest()
{
    QStringList expectedDescription;
    expectedDescription.append("Default BlinkyTape Firmware");
    expectedDescription.append("Use this for all standard functions");

    QCOMPARE(FirmwareStore::getFirmwareDescription(BLINKYTAPE_DEFAULT_FIRMWARE_NAME), expectedDescription);
}

void FirmwareStoreTests::getFirmwareDataDoesntExistTest()
{
    QVERIFY(FirmwareStore::getFirmwareData("thisfirmwaredoesntexist").data.isNull());
}

void FirmwareStoreTests::getFirmwareDataHasBlinkyTapeDefaultTest()
{
    QVERIFY(!FirmwareStore::getFirmwareData(BLINKYTAPE_DEFAULT_FIRMWARE_NAME).data.isNull());
}
