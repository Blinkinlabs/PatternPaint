#include <QImage>
#include <QString>

#ifndef PATTERN_H
#define PATTERN_H


/// Container for a compressed pattern
/// This class performs a 1-shot compression of an image from a QIMage.
class Pattern
{
public:
    enum Encoding {
        RGB24       = 0,     /// RGB24 mode (uncompressed 24 bit)
        RGB565_RLE  = 1,     /// RGB 565 + RLE mode (compressed 16 bit)
        INDEXED     = 2,     /// 8-bit indexed mode (pallated 8 bit)
        INDEXED_RLE = 3,
    };

    // Create an pattern from a QImage
    Pattern(QImage image, int frameDelay, Encoding encoding);

    // TODO: create an pattern from byte array

    Encoding encoding;  /// Encoding used to compress the pattern
    QImage image;       /// QImage representation of the pattern
    QByteArray data;    /// Byte array representation of the pattern
    QString header;     /// C++ header representation of the pattern

    int frameCount;     /// Number of frames in this pattern
    int ledCount;       /// Number of LEDs in this tape
    int frameDelay;     /// Length of time between frames of data, in ms

    int colorCount() const;  /// Number of unique colors in the original image

private:
    // Compress an RGB color to the 565 color space
    // TODO: Improve this conversion using a lookup table, instead of
    // decimation.
    int QRgbTo565(QRgb color);

    void encodeImageRGB24();
    void encodeImageRGB16_RLE();
    void encodeImageIndexed();
    void encodeImageIndexed_RLE();
};


#endif // PATTERN_H
