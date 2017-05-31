#include "blinkypendantuploaddata.h"

#include "bytearrayhelpers.h"

// TODO: push the image conversions into here so they are less awkward.
#define PIXEL_COUNT 10

QByteArray BlinkyPendantUploadData::makePatternTableHeader(uint8_t patternCount, DisplayMode displayMode) {
    QByteArray header;

    header.append((char)0x31);    // header
    header.append((char)0x23);
    header.append((char)patternCount);
    header.append((char)displayMode);
    return header;
}

QByteArray BlinkyPendantUploadData::makePatternTableEntry(uint32_t offset, uint16_t frameCount, uint16_t frameDelay)
{
    QByteArray entry;

    // Animation entry
    entry.append((char)0);             // Encoding type (1 byte) (RGB24, uncompressed) (TODO)
    entry += ByteArrayHelpers::uint32ToByteArrayBig(offset);        // Data offset (4 bytes)
    entry += ByteArrayHelpers::uint16ToByteArrayBig(frameCount);  // Frame count (2 bytes)
    entry += ByteArrayHelpers::uint16ToByteArrayBig(frameDelay);  // Frame delay (2 bytes)

    return entry;
}



bool BlinkyPendantUploadData::init(DisplayMode displayMode, const QList<PatternWriter> &patternWriters) {

    // Test for the minimum/maximum patterns count
    if (patternWriters.count() == 0) {
        errorString = QString("No Patterns detected!");
        return false;
    }

    // Test for the minimum/maximum patterns count
    if (patternWriters.count() > 255) {
        errorString = QString("Too many patterns, cannot fit in pattern table.");
        return false;
    }

    data.append(makePatternTableHeader(patternWriters.size(),
                                       displayMode));

    QByteArray patternData;

    for (PatternWriter pattern : patternWriters) {
        // Make sure we have an image compatible with the BlinkyPendant
        if (pattern.getLedCount() != 10) {
            errorString = "Wrong pattern size- must be 10 pixels high!";
            return false;
        }
        if (pattern.getEncoding() != PatternWriter::RGB24) {
            errorString = "Wrong encoding type- must be RGB24!";
            return false;
        }

        if(pattern.getFrameCount() > 65535) {
            errorString = "Pattern too long, must be < 65535 frames";
            return false;
        }

        if(pattern.getFrameDelay() > 65535) {
            errorString = "Frame delay too long, must be < 65535";
            return false;
        }

        data.append(makePatternTableEntry(patternData.length(),
                                          pattern.getFrameCount(),
                                          pattern.getFrameDelay()));

        // Make sure we have an image compatible with the BlinkyPendant
        patternData += pattern.getDataAsBinary();       // image data (RGB24, uncompressed)
    }

    data += patternData;

    return true;
}
