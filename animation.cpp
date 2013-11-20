#include "animation.h"
#include "blinkytape.h"

Animation::Animation(QImage image_, int frameDelay_, Encoding encoding_)
{
    image = image_;
    frameDelay = frameDelay_;
    encoding = encoding_;
    frameCount = image.width();
    ledCount = image.height();


    // Create a new encoder
    switch(encoding) {
    case Encoding_RGB565_RLE:
        encodeImageRGB16_RLE();
        break;
    case Encoding_RGB24:
        encodeImageRGB24();
    }
}

int Animation::QRgbTo565(QRgb color) {
    return (((qRed(color)   >> 3) & 0x1F)   << 11)
           | (((qGreen(color) >> 2) & 0x3F) <<  5)
           | (((qBlue(color)  >> 3) & 0x1F)      );
}

void Animation::encodeImageRGB16_RLE() {
    data.clear();

    for(int frame = 0; frame < image.width(); frame++) {
        int currentColor;
        int runCount = 0;

        for(int pixel = 0; pixel < image.height(); pixel++) {
            QRgb color = BlinkyTape::correctBrightness(image.pixel(frame, pixel));

            int decimatedColor = QRgbTo565(color);

            if(runCount == 0) {
                currentColor = decimatedColor;
            }

            if(currentColor != decimatedColor) {
                data.append(runCount);
                data.append((currentColor >> 8) & 0xFF);
                data.append((currentColor)      & 0xFF);

                runCount = 1;
                currentColor = decimatedColor;
            }
            else {
                runCount++;
            }
        }
        data.append(runCount);
        data.append((currentColor >> 8) & 0xFF);
        data.append((currentColor)      & 0xFF);
    }

    for(int i = 0; i < data.length()/3; i++) {
        qDebug() << "RunLength:" << (int)data[i*3]
                 << "Color:" << (int)data[i*3+1] << (int)data[i*3+2];
    }
}


void Animation::encodeImageRGB24() {
    data.clear();

    for(int frame = 0; frame < image.width(); frame++) {
        for(int pixel = 0; pixel < image.height(); pixel++) {
            QRgb color = BlinkyTape::correctBrightness(image.pixel(frame, pixel));
            data.append(qRed(color));
            data.append(qGreen(color));
            data.append(qBlue(color));
        }
    }
}
