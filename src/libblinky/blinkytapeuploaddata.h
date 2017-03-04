#ifndef AVRUPLOADDATA_H
#define AVRUPLOADDATA_H

#include <QByteArray>
#include <vector>
#include "patternwriter.h"
#include "memorysection.h"

#define FLASH_MEMORY_AVAILABLE          0x7000  // Amount of application space in the flash
#define FLASH_MEMORY_PAGE_SIZE_BYTES    0x80    // Size of a page of memory in our flash

QByteArray makePatternTableHeader(uint8_t patternCount, uint16_t ledCount);
QByteArray makePatternTableEntry(PatternWriter::Encoding encodig, uint16_t offset, uint16_t frameCount, uint16_t frameDelay);

QByteArray makeBrightnessTable(int maxBrightnessPercent);

/// Utility class for assembling flash sections for the BlinkyTape
class BlinkyTapeUploadData
{
public:
    bool init(const QString &firmwareName, const QList<PatternWriter> &patterns);

    QList<MemorySection> flashData;

    QString errorString;
};

#endif // AVRUPLOADDATA_H
