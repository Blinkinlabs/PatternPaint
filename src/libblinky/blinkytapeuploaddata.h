#ifndef AVRUPLOADDATA_H
#define AVRUPLOADDATA_H

#include "libblinkyglobal.h"

#include <QByteArray>
#include "patternwriter.h"
#include "memorysection.h"

#define FLASH_MEMORY_AVAILABLE          0x7000  // Amount of application space in the flash
#define FLASH_MEMORY_PAGE_SIZE_BYTES    0x80    // Size of a page of memory in our flash


//TODO: Why are these global? Either put in namespace or make static functions.
LIBBLINKY_EXPORT QByteArray makePatternTableHeader(uint8_t patternCount, uint16_t ledCount);

LIBBLINKY_EXPORT QByteArray makePatternTableEntry(PatternWriter::Encoding encodig, uint16_t offset, uint16_t frameCount, uint16_t frameDelay);

LIBBLINKY_EXPORT QByteArray makeBrightnessTable(int maxBrightnessPercent);

/// Utility class for assembling flash sections for the BlinkyTape
class LIBBLINKY_EXPORT BlinkyTapeUploadData
{
public:
    bool init(const QString &firmwareName, const QList<PatternWriter> &patterns);

    QList<MemorySection> flashData;

    QString errorString;
};

#endif // AVRUPLOADDATA_H
