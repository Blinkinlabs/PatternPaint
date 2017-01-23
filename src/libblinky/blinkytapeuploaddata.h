#ifndef AVRUPLOADDATA_H
#define AVRUPLOADDATA_H

#include <QByteArray>
#include <vector>
#include "patternwriter.h"
#include "memorysection.h"

#define FLASH_MEMORY_AVAILABLE          0x7000  // Amount of application space in the flash
#define FLASH_MEMORY_PAGE_SIZE_BYTES    0x80    // Size of a page of memory in our flash

/// Utility class for assembling flash sections for the BlinkyTape
class BlinkyTapeUploadData
{
public:
    bool init(QList<PatternWriter> &patterns);

    MemorySection sketchSection;
    MemorySection patternDataSection;
    MemorySection patternTableSection;

    QString errorString;
};

#endif // AVRUPLOADDATA_H
