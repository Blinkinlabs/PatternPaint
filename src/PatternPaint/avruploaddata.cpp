#include "avruploaddata.h"
#include "PatternPlayer_Sketch.h"
#include "blinkytape.h"

#define BUFF_LENGTH 100

#define PATTERN_TABLE_HEADER_LENGTH     2
#define PATTERN_TABLE_ENTRY_LENGTH      7


bool avrUploadData::init(std::vector<Pattern> patterns) {
    char buff[BUFF_LENGTH];
    QString errorString;

    // We need to build two things- a memory image containing the sketch and pattern data,
    // and a memory image containing the pattern data information table.

    sketch = QByteArray();          // Program data
    patternData = QByteArray();     // Pattern Data
    patternTable = QByteArray();    // Pattern data header

    sketch.append(PatternPlayerSketch, sizeof(PatternPlayerSketch));

    // Expand sketch size to FLASH_MEMORY_PAGE_SIZE boundary
    while(sketch.length() % FLASH_MEMORY_PAGE_SIZE != 0) {
        sketch.append(static_cast<char>(0xFF));
    }


    // Test for the minimum/maximum patterns size
    if(patterns.size() == 0) {
        errorString = QString("No Patterns detected!");
        return false;
    }
    if(patterns.size() >= ((FLASH_MEMORY_PAGE_SIZE - PATTERN_TABLE_HEADER_LENGTH) / PATTERN_TABLE_ENTRY_LENGTH)) {
        errorString = QString("Too many patterns, cannot fit in pattern table.");
        return false;
    }

    snprintf(buff, BUFF_LENGTH, "Building pattern array. Pattern Count: %zu, led count: %i",
             patterns.size(),
             patterns[0].ledCount);
    qDebug() << buff;

    patternTable.append(static_cast<char>(patterns.size()));       // First byte of the metadata is how many patterns there are
    patternTable.append(static_cast<char>(patterns[0].ledCount));  // Second byte is the length of the LED strip
    // TODO: make the LED count to a separate, explicit parameter?

    int dataOffset = sketch.length();

    // Now, for each pattern, append the image data to the sketch
    for(std::vector<Pattern>::iterator pattern = patterns.begin(); pattern != patterns.end(); ++pattern) {
        snprintf(buff, BUFF_LENGTH, "Adding pattern. Encoding: %x, framecount: %i, frameDelay: %i, size: %iB, offset: %iB",
                 pattern->encoding,
                 pattern->frameCount,
                 pattern->frameDelay,
                 pattern->data.length(),
                 dataOffset);
        qDebug() << buff;

        // Build the table entry for this pattern
        patternTable.append(static_cast<char>((pattern->encoding) & 0xFF));             // Offset 0: encoding (1 byte)
        patternTable.append(static_cast<char>((dataOffset >> 8) & 0xFF));               // Offset 1: memory location (2 bytes)
        patternTable.append(static_cast<char>((dataOffset     ) & 0xFF));
        patternTable.append(static_cast<char>((pattern->frameCount >> 8  ) & 0xFF));    // Offset 3: frame count (2 bytes)
        patternTable.append(static_cast<char>((pattern->frameCount       ) & 0xFF));
        patternTable.append(static_cast<char>((pattern->frameDelay >> 8  ) & 0xFF));    // Offset 5: frame delay (2 bytes)
        patternTable.append(static_cast<char>((pattern->frameDelay       ) & 0xFF));

        // and append the image data
        patternData += pattern->data;
        dataOffset += pattern->data.length();
    }

    // Pad pattern table to FLASH_MEMORY_PAGE_SIZE bytes.
    while(patternTable.length() < FLASH_MEMORY_PAGE_SIZE) {
        patternTable.append(static_cast<char>(0xFF));
    }

    snprintf(buff, BUFF_LENGTH, "Sketch size: %iB, pattern data size: %iB, pattern table size: %iB",
             sketch.length(),
             patternData.length(),
             patternTable.length());
    qDebug() << buff;


    sketchAddress = FLASH_MEMORY_SKETCH_ADDRESS;
    patternDataAddress = FLASH_MEMORY_SKETCH_ADDRESS + sketch.length();
    patternTableAddress = FLASH_MEMORY_PATTERN_TABLE_ADDRESS;

    return true;
}

