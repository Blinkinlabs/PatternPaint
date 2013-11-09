#include "ledwriter.h"

#include <fstream>
#include <sstream>
#include <string>


/// Base class for our image encoder
/// Actually we just have two kinds, uncompressed and
class Encoder
{
public:
    // Set the animation that this encoder should use
    virtual void setAnimation(QImage animation) = 0;

    // Size of the animation after compression, in bytes
    virtual int getAnimationSizeBytes() = 0;

    // Write the animation out as a c++ declaration
    virtual void getAnimationAsHeader(std::ofstream &stream) = 0;

    // Get a byte array that represents the animation data
    // This is intended to be packed into a .hex file directly
    // The format of this is encoder specific, so the encoder implementation
    // here should line up with the encoder implementation in the
    // Arduino library.
    virtual std::vector<char> getAnimationDataAsBytes() = 0;
};

class RGB24_encoder : public Encoder {
public:
    void setAnimation(QImage animation);

    int getAnimationSizeBytes();

    void getAnimationAsHeader(std::ofstream &stream);

    std::vector<char> getAnimationDataAsBytes();
private:
    QImage m_animation;
};


void RGB24_encoder::setAnimation(QImage animation) {
    m_animation = animation;
}

int RGB24_encoder::getAnimationSizeBytes() {
    // 3 bytes per pixel
    return m_animation.width() * m_animation.height() * 3;
}

void RGB24_encoder::getAnimationAsHeader(std::ofstream &stream) {
    const int bufferLength = 100;
    char buffer[100];

    stream << "const PROGMEM prog_uint8_t povData[] =\n";
    stream << "{\n";

    // The RGB encoder just stores the data as R,G,B over and over again.
    for(int frame = 0; frame < m_animation.width(); frame++) {
        for(int pixel = 0; pixel < m_animation.height(); pixel++) {
            int color = m_animation.pixel(frame, pixel);
            snprintf(buffer,bufferLength,"  %3i, %3i, %3i,\n",
                     qRed(color),
                     qGreen(color),
                     qBlue(color));
            stream << buffer;
        }
    }
    stream << "};\n";

    snprintf(buffer,bufferLength,"Animation pov(%i, povData, ENCODING_NONE, %i);\n",
             m_animation.width(),m_animation.height());
    stream << buffer;
}

std::vector<char> RGB24_encoder::getAnimationDataAsBytes() {
    std::vector<char> data;

    // The RGB encoder just stores the data as R,G,B over and over again.
    for(int frame = 0; frame < m_animation.width(); frame++) {
        for(int pixel = 0; pixel < m_animation.height(); pixel++) {
            int color = m_animation.pixel(frame, pixel);
            data.push_back(qRed(color));
            data.push_back(qGreen(color));
            data.push_back(qBlue(color));
        }
    }

    return data;
}

LedWriter::LedWriter(int ledCount)
{
    m_ledCount = ledCount;
}

void LedWriter::loadAnimation(QImage animation, Encoding encoding)
{
    m_animation = animation;

    // Create a new encoder
    switch(encoding) {
    case Encoding_RGB16_RLE:
        break;
    case Encoding_RGB24:
    default:
        // TODO: Default case fails?
        m_encoder = new RGB24_encoder();
        break;
    }

    m_encoder->setAnimation(animation);
}

void LedWriter::makeCHeader() {
    std::ofstream stream;
    // TODO: Don't hardcode this filename, of course.
    stream.open("/Users/mattmets/Desktop/image.h");

    // Record some basic information about the animation
    stream << "// Data file for animation " << m_animationName <<std::endl;
    stream << "// Compression: " << std::endl;
    // TODO: Don't hardcode this here?
    stream << "// Uncompressed size:" << m_animation.height()*m_animation.width()*3 << std::endl;
    stream << "// Compressed size:" << m_encoder->getAnimationSizeBytes() << std::endl;

    m_encoder->getAnimationAsHeader(stream);

    stream.close();
}

void LedWriter::makeBinary() {
    // TODO: Implmement me!
}
