#include "blinkytapeuploaddata.h"

#include "blinkytape.h"
#include "firmwarestore.h"

#include "bytearrayhelpers.h"

#include <QDebug>
#include <QSettings>
#include <QStandardPaths>
#include <QFileDialog>

#define PATTERN_TABLE_HEADER_LENGTH_BYTES     3
#define PATTERN_TABLE_ENTRY_LENGTH_BYTES      7

#define FLASH_MEMORY_PATTERN_TABLE_ADDRESS (FLASH_MEMORY_AVAILABLE - FLASH_MEMORY_PAGE_SIZE_BYTES) // Location of pattern table


#define BLINKYTAPE_MAX_BRIGHTNESS_DEFAULT 36

bool BlinkyTapeUploadData::init(const QString &firmwareName, QList<PatternWriter> &patterns)
{
    // First, build the flash section for the sketch. This is the same for
    // all uploads
    MemorySection sketchSection = FirmwareStore::getFirmwareData(firmwareName);

    if(sketchSection.data.isNull()) {
        errorString = "Firmware read failed!";
        return false;
    }

    // Expand sketch size to FLASH_MEMORY_PAGE_SIZE_BYTES boundary
    padToBoundary(sketchSection.data, FLASH_MEMORY_PAGE_SIZE_BYTES);

    // Next, build the pattern data section and pattern header table


    QByteArray patternData;     // Pattern Data
    QByteArray patternTable;    // Pattern data header

    // Test for the minimum/maximum patterns count
    if (patterns.count() == 0) {
        errorString = QString("No Patterns detected!");
        return false;
    }
    if (patterns.count()
        >= ((FLASH_MEMORY_PAGE_SIZE_BYTES - PATTERN_TABLE_HEADER_LENGTH_BYTES) / PATTERN_TABLE_ENTRY_LENGTH_BYTES)) {
        errorString = QString("Too many patterns, cannot fit in pattern table.");
        return false;
    }

    qDebug() << "Building pattern array"
             << "Pattern count:" << patterns.count()
             << "Led count:" << patterns.first().getLedCount();

    patternTable.append(static_cast<char>(patterns.count()));       // Offset 0: Pattern count (1 byte)
    // TODO: make the LED count to a separate, explicit parameter?

    // TODO: Test that the LED length is in range

    patternTable += uint16ToByteArrayLittle(patterns.first().getLedCount());  // Offset 1: Number of LEDs connected to the controller (2 bytes)

    // TODO: Pass this in somehow.
    QSettings settings;
    float maxBrightness = settings.value("BlinkyTape/maxBrightness", BLINKYTAPE_MAX_BRIGHTNESS_DEFAULT).toInt();

    const uint8_t brightnessSteps = 8;
    float brightnessStepModifiers[brightnessSteps] = {255,191,114,41,20,41,114,191};

    for(int i = 0; i < brightnessSteps; i++)
        patternTable.append(static_cast<char>(brightnessStepModifiers[i]*maxBrightness/100));  // Offset 2: Brightness steps (8 bytes)

    int dataOffset = sketchSection.extent();

    // Now, for each pattern, append the image data to the sketch
    for (PatternWriter pattern : patterns) {
        qDebug() << "Adding pattern "
                 << "Encoding:" << pattern.getEncoding()
                 << "Frame count:" << pattern.getFrameCount()
                 << "Frame delay:" << pattern.getFrameDelay()
                 << "Count:" << pattern.getDataAsBinary().length()
                 << "Offset:" << dataOffset;

        // TOD: Test that all the values are in range

        // Build the table entry for this pattern
        patternTable.append((char)((pattern.getEncoding()) & 0xFF));   // Offset 0: encoding (1 byte)
        patternTable += uint16ToByteArrayLittle(dataOffset);                         // Offset 1: memory location (2 bytes)
        patternTable += uint16ToByteArrayLittle(pattern.getFrameCount());           // Offset 3: frame count (2 bytes)
        patternTable += uint16ToByteArrayLittle(pattern.getFrameDelay());           // Offset 5: frame delay (2 bytes)

        // and append the image data
        patternData += pattern.getDataAsBinary();
        dataOffset += pattern.getDataAsBinary().count();
    }

    // Pad pattern table to FLASH_MEMORY_PAGE_SIZE_BYTES.
    padToBoundary(patternTable, FLASH_MEMORY_PAGE_SIZE_BYTES);

    flashData.append(sketchSection);
    flashData.append(MemorySection("PatternData",
                                   sketchSection.address + sketchSection.data.count(),
                                   patternData));
    flashData.append(MemorySection("PatternTable",
                                   FLASH_MEMORY_PATTERN_TABLE_ADDRESS,
                                   patternTable));

    for(MemorySection &section : flashData)
        qDebug() << "Section"
                 << "name:" << section.name
                 << "address:" << section.address
                 << "size:" << section.data.count();

    return true;
}
