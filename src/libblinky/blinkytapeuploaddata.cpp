#include "blinkytapeuploaddata.h"

#include "firmwarestore.h"

#include "bytearrayhelpers.h"

#include <QDebug>
#include <QSettings>
#include <QStandardPaths>
#include <QFileDialog>

#include <cmath>

#define PATTERN_TABLE_HEADER_LENGTH_BYTES     3
#define BRIGHTNESS_TABLE_LENGTH_BYTES     8
#define PATTERN_TABLE_ENTRY_LENGTH_BYTES      7

// Note: This is limited by the 1-byte header field to 255, max.
// It is further limited by the pattern size table in the EEPROM
#define MAX_PATTERN_COUNT 50

#define FLASH_MEMORY_PATTERN_TABLE_ADDRESS (FLASH_MEMORY_AVAILABLE - FLASH_MEMORY_PAGE_SIZE_BYTES) // Location of pattern table

#define EEPROM_SETTINGS_ADDRESS (0x0)
#define EEPROM_PATTERN_TABLE_ADDRESS (0x100)

#define BLINKYTAPE_MAX_BRIGHTNESS_DEFAULT 36
#define BLINKYTAPE_BUTTON_SHORT_PRESS_DEFAULT 1
#define BLINKYTAPE_BUTTON_LONG_PRESS_DEFAULT 2

QByteArray makeBrightnessTable(int maxBrightnessPercent) {
    QByteArray brightnessTable;

    if (maxBrightnessPercent < 0)
        maxBrightnessPercent = 0;

    if (maxBrightnessPercent > 100)
        maxBrightnessPercent = 100;

    float maxBrightness = maxBrightnessPercent/100.0;
    QList<int> brightnessStepModifiers;

    brightnessStepModifiers.append(QList<int>{255,191,114,41,20,41,114,191});

    for(int brightnessStepModifier : brightnessStepModifiers)
        brightnessTable.append(static_cast<char>(round(brightnessStepModifier*maxBrightness)));

    return brightnessTable;
}

QByteArray makePatternTableEntry(PatternWriter::Encoding encoding, uint16_t offset, uint16_t frameCount, uint16_t frameDelay, uint16_t repeatCount) {
    QByteArray entry;

    entry.append((char)((encoding) & 0xFF));            // Offset 0: encoding (1 byte)
    entry.append(ByteArrayHelpers::uint16ToByteArrayLittle(offset));      // Offset 1: memory location (2 bytes)
    entry.append(ByteArrayHelpers::uint16ToByteArrayLittle(frameCount));  // Offset 3: frame count (2 bytes)
    entry.append(ByteArrayHelpers::uint16ToByteArrayLittle(frameDelay));  // Offset 5: frame delay (2 bytes)
    entry.append(ByteArrayHelpers::uint16ToByteArrayLittle(repeatCount));  // Offset 7: repeat count (2 bytes)

    return entry;
}

bool BlinkyTapeUploadData::init(const QString &firmwareName, const QList<PatternWriter> &patterns)
{
    // TODO: Pass this in somehow.
    QSettings settings;
    const int maxBrightness = settings.value("BlinkyTape/maxBrightness", BLINKYTAPE_MAX_BRIGHTNESS_DEFAULT).toInt();

    const int buttonShortPressBehaviour = settings.value("BlinkyTape/buttonShortPress", BLINKYTAPE_BUTTON_SHORT_PRESS_DEFAULT).toInt();
    const int buttonLongPressBehaviour = settings.value("BlinkyTape/buttonLongPress", BLINKYTAPE_BUTTON_LONG_PRESS_DEFAULT).toInt();

    // First, build the flash section for the sketch. This is the same for
    // all uploads
    MemorySection sketchSection = FirmwareStore::getFirmwareData(firmwareName);

    if(sketchSection.data.isNull()) {
        errorString = "Firmware read failed!";
        return false;
    }

    // Test for the minimum/maximum patterns count
    if (patterns.count() == 0) {
        errorString = QString("No Patterns detected!");
        return false;
    }

    if (patterns.count() > MAX_PATTERN_COUNT) {
        errorString = QString("Too many patterns, cannot fit in pattern table.");
        return false;
    }

    // Expand sketch size to FLASH_MEMORY_PAGE_SIZE_BYTES boundary
    ByteArrayHelpers::padToBoundary(sketchSection.data, FLASH_MEMORY_PAGE_SIZE_BYTES);


    // Build the settings table
    qDebug() << "Building EEPROM settings"
             << "button short press:" << buttonShortPressBehaviour
             << "button long press:" << buttonLongPressBehaviour;

    // TODO: make the LED count to a separate, explicit parameter?
    // TODO: Test that the LED length is in range
    const uint16_t ledCount = patterns.first().getLedCount();
    QByteArray currentSettings;
    currentSettings.append((uint8_t)0); // current pattern index
    currentSettings.append((uint8_t)0); // current brightness index
    currentSettings.append((uint8_t)buttonShortPressBehaviour);
    currentSettings.append((uint8_t)buttonLongPressBehaviour);

    currentSettings.append((uint8_t)patterns.count());                                       // Pattern count (1 byte)
    currentSettings.append(ByteArrayHelpers::uint16ToByteArrayLittle(ledCount));    // Number of LEDs connected to the controller (2 bytes)
    currentSettings.append(makeBrightnessTable(maxBrightness));

    qDebug() << "Building pattern array"
             << "Pattern count:" << patterns.count()
             << "Led count:" << patterns.first().getLedCount();


    // Next, build the pattern data section and pattern header table
    QByteArray patternTable;    // Pattern data header
    QByteArray patternData;     // Pattern Data

    unsigned int patternDataAddress = sketchSection.address + sketchSection.data.size();

    // Now, for each pattern, append the image data to the sketch
    for (const PatternWriter& pattern : patterns) {
        qDebug() << "Adding pattern "
                 << "Encoding:" << pattern.getEncoding()
                 << "Frame count:" << pattern.getFrameCount()
                 << "Frame delay:" << pattern.getFrameDelay()
                 << "Size:" << pattern.getDataAsBinary().size()
                 << "Offset:" << patternDataAddress + patternData.size();

        // TOD: Test that all the values are in range

        // Build the table entry for this pattern
        patternTable.append(makePatternTableEntry(pattern.getEncoding(),
                                                  patternDataAddress + patternData.size(),
                                                  pattern.getFrameCount(),
                                                  pattern.getFrameDelay(),
                                                  pattern.getRepeatCount()));

        // and append the image data
        patternData += pattern.getDataAsBinary();
    }


    flashData.append(sketchSection);
    flashData.append(MemorySection("PatternData",
                                   patternDataAddress,
                                   patternData));

    // Set the default pattern/brightness settings
    eepromData.append(MemorySection("currentSettings",
                                   EEPROM_SETTINGS_ADDRESS,
                                   currentSettings));

    eepromData.append(MemorySection("PatternTable",
                                   EEPROM_PATTERN_TABLE_ADDRESS,
                                   patternTable));

    for(MemorySection &section : flashData)
        qDebug() << "Flash Section"
                 << "name:" << section.name
                 << "address:" << section.address
                 << "size:" << section.data.size();

    for(MemorySection &section : eepromData)
        qDebug() << "EEPROM Section"
                 << "name:" << section.name
                 << "address:" << section.address
                 << "size:" << section.data.size();

    return true;
}
