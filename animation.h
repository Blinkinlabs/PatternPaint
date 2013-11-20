#include <QImage>
#include <string>

#ifndef ANIMATION_H
#define ANIMATION_H


/// Container for a compressed animation
class Animation
{
public:
    enum Encoding {
        Encoding_RGB24      = 0,
        Encoding_RGB565_RLE = 1,
    };

    // Create an animation from a QImage
    Animation(QImage image_, int frameDelay_, Encoding encoding_ = Encoding_RGB24);

    Encoding encoding;
    QImage image;
    QByteArray data;
    int frameCount;     /// Number of animation frames in this animation
    int ledCount;       /// Number of LEDs in this tape
    int frameDelay;     /// Length of time between frames of data, in ms

private:
    // Compress an RGB color to the 565 color space
    // TODO: Improve this conversion using a lookup table, instead of
    // decimation.
    int QRgbTo565(QRgb color);

    // Encode the image using the RGB24 format
    void encodeImageRGB24();
    void encodeImageRGB16_RLE();
};


#endif // ANIMATION_H
