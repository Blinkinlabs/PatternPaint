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
    };

    // Compute the bitstream for a pattern
    PatternWriter(const Pattern &pattern, const Fixture &fixture, Encoding encoding);

    Encoding getEncoding() const;
    const QByteArray &getDataAsBinary() const;
    const QString &getDataAsHeader() const;

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

    // Compress an RGB color to the 565 color space
    // TODO: Improve this conversion using a lookup table, instead of
    // decimation.
    int QRgbTo565(const QColor &color, ColorMode colorMode);

    void encodeImageRGB24(const Pattern &pattern, const Fixture &fixture);
    void encodeImageRGB565_RLE(const Pattern &pattern, const Fixture &fixture);
};

#endif // PATTERN_WRITER_H
