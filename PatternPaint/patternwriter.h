#ifndef PATTERN_WRITER_H
#define PATTERN_WRITER_H

#include "pattern.h"
#include "fixture.h"

#include <QImage>
#include <QString>

/// Container for a compressed pattern
/// This class performs a 1-shot compression of an image from a QIMage.
class PatternWriter
{
public:
    enum Encoding {
        RGB24 = 0,           /// RGB24 mode (uncompressed 24 bit)
        RGB565_RLE = 1,      /// RGB 565 + RLE mode (compressed 16 bit)
// INDEXED     = 2,     /// 8-bit indexed mode (pallated 8 bit)
// INDEXED_RLE = 3,     /// 8-bit indexed mode + RLE (pallated 8 bit)
    };

    // Create an pattern from a QImage
    PatternWriter(const Pattern *pattern, Encoding encoding, Fixture *fixture);

    Encoding getEncoding() const;
    QByteArray getData() const;
    QString getHeader() const;

    int colorCount() const;  /// Number of unique colors in the original image

    int getFrameCount() const;
    int getLedCount() const;
    int getFrameDelay() const;

private:

    Encoding encoding;  /// Encoding used to compress the pattern
    QByteArray data;    /// Byte array representation of the pattern
    QString header;     /// C++ header representation of the pattern

    int frameCount;     /// Number of frames in this pattern
    int ledCount;       /// Number of LEDs attached to this blinky
    int frameDelay;     /// Length of time between frames of data, in ms
    QPointer<Fixture> fixture;  /// Fixture we are writing to

    // Compress an RGB color to the 565 color space
    // TODO: Improve this conversion using a lookup table, instead of
    // decimation.
    int QRgbTo565(QColor color);

    void encodeImageRGB24(const Pattern *pattern);
    void encodeImageRGB565_RLE(const Pattern *pattern);
// void encodeImageIndexed(const Pattern *pattern);
// void encodeImageIndexed_RLE(const Pattern *pattern);
};

#endif // PATTERN_WRITER_H
