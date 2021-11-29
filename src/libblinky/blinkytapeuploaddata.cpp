#include "blinkytapeuploaddata.h"

#include "blinkytape.h"
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

#define EEPROM_CURRENT_SETTINGS_SIZE (2)
#define EEPROM_CURRENT_SETTINGS_ADDRESS (0x0)
#define EEPROM_PATTERN_TABLE_ADDRESS (0x100)

#define BLINKYTAPE_MAX_BRIGHTNESS_DEFAULT 36


QByteArray makePatternTableHeader(uint8_t patternCount, uint16_t ledCount) {
    QByteArray header;

    header.append(patternCount);                        // Offset 0: Pattern count (1 byte)
    header.append(ByteArrayHelpers::uint16ToByteArrayLittle(ledCount));   // Offset 1: Number of LEDs connected to the controller (2 bytes)

    return header;
}

QByteArray makePatternTableEntry(PatternWriter::Encoding encoding, uint16_t offset, uint16_t frameCount, uint16_t frameDelay) {
    QByteArray entry;

    entry.append((char)((encoding) & 0xFF));            // Offset 0: encoding (1 byte)
    entry.append(ByteArrayHelpers::uint16ToByteArrayLittle(offset));      // Offset 1: memory location (2 bytes)
    entry.append(ByteArrayHelpers::uint16ToByteArrayLittle(frameCount));  // Offset 3: frame count (2 bytes)
    entry.append(ByteArrayHelpers::uint16ToByteArrayLittle(frameDelay));  // Offset 5: frame delay (2 bytes)

    return entry;
}

QByteArray makeBrightnessTable(int maxBrightnessPercent) {
    QByteArray brightnessTable;

    if (maxBrightnessPercent < 0)
        maxBrightnessPercent = 0;

    if (maxBrightnessPercent > 100)
        maxBrightnessPercent = 100;

    float maxBrightness = maxBrightnessPercent/100.0;
    QList<int> brightnessStepModifiers = {255,191,114,41,20,41,114,191};

    for(int brightnessStepModifier : brightnessStepModifiers)
        brightnessTable.append(static_cast<char>(round(brightnessStepModifier*maxBrightness)));

    return brightnessTable;
}

bool BlinkyTapeUploadData::init(const QString &firmwareName, const QList<PatternWriter> &patterns)
{
    // TODO: Pass this in somehow.
    QSettings settings;
    int maxBrightness = settings.value("BlinkyTape/maxBrightness", BLINKYTAPE_MAX_BRIGHTNESS_DEFAULT).toInt();

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

    // Next, build the pattern data section and pattern header table

    qDebug() << "Building pattern array"
             << "Pattern count:" << patterns.count()
             << "Led count:" << patterns.first().getLedCount();

    QByteArray patternTable;    // Pattern data header
    QByteArray patternData;     // Pattern Data

    // TODO: make the LED count to a separate, explicit parameter?
    // TODO: Test that the LED length is in range

    patternTable.append(makePatternTableHeader(patterns.count(), patterns.first().getLedCount()));
    patternTable.append(makeBrightnessTable(maxBrightness));

    unsigned int patternDataAddress = sketchSection.address + sketchSection.data.count();

    // Now, for each pattern, append the image data to the sketch
    for (PatternWriter pattern : patterns) {
        qDebug() << "Adding pattern "
                 << "Encoding:" << pattern.getEncoding()
                 << "Frame count:" << pattern.getFrameCount()
                 << "Frame delay:" << pattern.getFrameDelay()
                 << "Count:" << pattern.getDataAsBinary().length()
                 << "Offset:" << patternDataAddress + patternData.count();

        // TOD: Test that all the values are in range

        // Build the table entry for this pattern
        patternTable.append(makePatternTableEntry(pattern.getEncoding(),
                                                  patternDataAddress + patternData.count(),
                                                  pattern.getFrameCount(),
                                                  pattern.getFrameDelay()));

        // and append the image data
        patternData += pattern.getDataAsBinary();
    }

    flashData.append(sketchSection);
    flashData.append(MemorySection("PatternData",
                                   patternDataAddress,
                                   patternData));

    // TODO: Zero out the beginning of the EEPROM, to set the default pattern/brightness settings
    const QByteArray eepromBytes(EEPROM_CURRENT_SETTINGS_SIZE, char(255));
    eepromData.append(MemorySection("currentSettings",
                                   EEPROM_CURRENT_SETTINGS_ADDRESS,
                                   eepromBytes));

    eepromData.append(MemorySection("PatternTable",
                                   EEPROM_PATTERN_TABLE_ADDRESS,
                                   patternTable));

    for(MemorySection &section : flashData)
        qDebug() << "Flash Section"
                 << "name:" << section.name
                 << "address:" << section.address
                 << "size:" << section.data.count();

    for(MemorySection &section : eepromData)
        qDebug() << "EEPROM Section"
                 << "name:" << section.name
                 << "address:" << section.address
                 << "size:" << section.data.count();

    return true;
}
