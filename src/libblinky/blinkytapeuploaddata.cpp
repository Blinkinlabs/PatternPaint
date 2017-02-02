#include "blinkytapeuploaddata.h"

#include "PatternPlayer_Sketch.h"
#include "blinkytape.h"
#include "firmwareimport.h"

#include <QDebug>
#include <QSettings>
#include <QStandardPaths>
#include <QFileDialog>

#define BUFF_LENGTH 100

#define PATTERN_TABLE_HEADER_LENGTH     3
#define PATTERN_TABLE_ENTRY_LENGTH      7

#define FLASH_MEMORY_SKETCH_ADDRESS     0x0000  // Location of sketch
#define FLASH_MEMORY_PATTERN_TABLE_ADDRESS (FLASH_MEMORY_AVAILABLE - FLASH_MEMORY_PAGE_SIZE_BYTES) // Location of pattern table


#define BLINKYTAPE_MAX_BRIGHTNESS_DEFAULT 36

QByteArray encodeWordLSB(int data)
{
    QByteArray output;
    output.append(static_cast<char>((data) & 0xFF));
    output.append(static_cast<char>((data >> 8) & 0xFF));
    return output;
}

bool BlinkyTapeUploadData::init(const QString &firmwareName, QList<PatternWriter> &patterns)
{
    char buff[BUFF_LENGTH];

    // First, build the flash section for the sketch. This is the same for
    // all uploads

    QByteArray sketch;          // Program data

    qDebug() << "Selected firmware: " << firmwareName;
    if(firmwareName == DEFAULT_FIRMWARE_NAME){
        sketch.append(reinterpret_cast<const char *>(PATTERNPLAYER_DATA), sizeof(PATTERNPLAYER_DATA));
    }else{
        // search for third party Firmware
        QString documents = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        documents.append(FIRMWARE_FOLDER);
        documents.append(firmwareName);
        QDir firmwareDir(documents);

        if (firmwareDir.exists()){
            documents.append("/");
            documents.append(firmwareName);
            documents.append(".hex");

            FirmwareReader newFirmware;

            if(newFirmware.load(documents)){
                qDebug("Firmware successfully read");
                sketch.append(newFirmware.getData());
            }else{
                snprintf(buff, BUFF_LENGTH,"Firmware read failed!");
                qDebug() << buff;
                errorString = buff;
                return false;
            }
        }
    }


    // Expand sketch size to FLASH_MEMORY_PAGE_SIZE_BYTES boundary
    while (sketch.length() % FLASH_MEMORY_PAGE_SIZE_BYTES != 0)
        sketch.append(static_cast<char>(0xFF));

    sketchSection = MemorySection("Sketch",
                                 FLASH_MEMORY_SKETCH_ADDRESS,
                                 sketch);



    // Next, build the pattern data section and pattern header table


    QByteArray patternData;     // Pattern Data
    QByteArray patternTable;    // Pattern data header

    // Test for the minimum/maximum patterns count
    if (patterns.count() == 0) {
        errorString = QString("No Patterns detected!");
        return false;
    }
    if (patterns.count()
        >= ((FLASH_MEMORY_PAGE_SIZE_BYTES - PATTERN_TABLE_HEADER_LENGTH) / PATTERN_TABLE_ENTRY_LENGTH)) {
        errorString = QString("Too many patterns, cannot fit in pattern table.");
        return false;
    }

    snprintf(buff, BUFF_LENGTH, "Building pattern array. Pattern Count: %i, led count: %i",
             patterns.count(),
             patterns.first().getLedCount());
    qDebug() << buff;

    patternTable.append(static_cast<char>(patterns.count()));       // Offset 0: Pattern count (1 byte)
    // TODO: make the LED count to a separate, explicit parameter?
    patternTable += encodeWordLSB(patterns.first().getLedCount());  // Offset 1: Number of LEDs connected to the controller (2 bytes)

    // TODO: Pass this in somehow.
    QSettings settings;
    float maxBrightness = settings.value("BlinkyTape/maxBrightness", BLINKYTAPE_MAX_BRIGHTNESS_DEFAULT).toInt();

    const uint8_t brightnessSteps = 8;
    float brightnessStepModifiers[brightnessSteps] = {255,191,114,41,20,41,114,191};

    for(int i = 0; i < brightnessSteps; i++)
        patternTable.append(static_cast<char>(brightnessStepModifiers[i]*maxBrightness/100));  // Offset 2: Brightness steps (8 bytes)

    int dataOffset = sketch.length();

    // Now, for each pattern, append the image data to the sketch
    foreach(PatternWriter pattern, patterns) {
        snprintf(buff, BUFF_LENGTH,
                 "Adding pattern. Encoding: %x, framecount: %i, frameDelay: %i, count: %iB, offset: %iB",
                 pattern.getEncoding(),
                 pattern.getFrameCount(),
                 pattern.getFrameDelay(),
                 pattern.getData().length(),
                 dataOffset);
        qDebug() << buff;

        // Build the table entry for this pattern
        patternTable.append((char)((pattern.getEncoding()) & 0xFF));   // Offset 0: encoding (1 byte)
        patternTable += encodeWordLSB(dataOffset);                         // Offset 1: memory location (2 bytes)
        patternTable += encodeWordLSB(pattern.getFrameCount());           // Offset 3: frame count (2 bytes)
        patternTable += encodeWordLSB(pattern.getFrameDelay());           // Offset 5: frame delay (2 bytes)

        // and append the image data
        patternData += pattern.getData();
        dataOffset += pattern.getData().count();
    }

    // Pad pattern table to FLASH_MEMORY_PAGE_SIZE_BYTES.
    while (patternTable.count() < FLASH_MEMORY_PAGE_SIZE_BYTES)
        patternTable.append(static_cast<char>(0xFF));

    snprintf(buff, BUFF_LENGTH, "Sketch size: %iB, pattern data size: %iB, pattern table size: %iB",
             sketch.count(),
             patternData.count(),
             patternTable.count());
    qDebug() << buff;

    patternDataSection = MemorySection("PatternData",
                                      FLASH_MEMORY_SKETCH_ADDRESS + sketch.count(),
                                      patternData);

    patternTableSection = MemorySection("PatternTable",
                                       FLASH_MEMORY_PATTERN_TABLE_ADDRESS,
                                       patternTable);
    return true;
}
