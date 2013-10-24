#include <QImage>
#include <string>

#ifndef LEDWRITER_H
#define LEDWRITER_H

// This is a private class
class Encoder;

// Write an LED animation out to file
class LedWriter
{
public:
    enum Encoding {
        Encoding_RGB24,
        Encoding_RGB16_RLE,
#ifndef Q_QDOC
        NEncodings
#endif
    };

    LedWriter(int ledCount = 60);

    // TODO: Destructor that kills the encoder

    void loadAnimation(QImage animation, Encoding encoding = Encoding_RGB24);

    // Make a C/C++ compatible header representing the animation
    void makeCHeader();

    // Make a C/C++ compatible header representing the animation
    void makeBinary();

private:
    int m_ledCount;  // Length of the LED string to draw this animation on

    std::string m_animationName;  // String name of the animation
    QImage m_animation;  // Bitmap of the animation, each column is a frame.

    Encoder* m_encoder;
};

#endif // LEDWRITER_H
