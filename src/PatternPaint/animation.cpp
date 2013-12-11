#include "animation.h"
#include "colormodel.h"

Animation::Animation(QImage image_, int frameDelay_, Encoding encoding_) :
    image(image_),
    frameDelay(frameDelay_),
    encoding(encoding_)
{
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
    header.clear(); // TODO: Move the header builder somewhere else?

    header.append("const PROGMEM prog_uint8_t animationData[]  = {\n");

    for(int frame = 0; frame < image.width(); frame++) {
        int currentColor;
        int runCount = 0;

        for(int pixel = 0; pixel < image.height(); pixel++) {
            QRgb color = ColorModel::correctBrightness(image.pixel(frame, pixel));

            int decimatedColor = QRgbTo565(color);

            if(runCount == 0) {
                currentColor = decimatedColor;
            }

            if(currentColor != decimatedColor) {
                int highByte = (currentColor >> 8) & 0xFF;
                int lowByte  = (currentColor)      & 0xFF;

                data.append(runCount);
                data.append(highByte);
                data.append(lowByte);

                header.append(QString("  %1, 0x%2, 0x%3,\n")
                              .arg(runCount, 3)
                              .arg(highByte, 2, 16, QLatin1Char('0'))
                              .arg(lowByte,  2, 16, QLatin1Char('0')));

                runCount = 1;
                currentColor = decimatedColor;
            }
            else {
                runCount++;
            }
        }

        int highByte = (currentColor >> 8) & 0xFF;
        int lowByte  = (currentColor)      & 0xFF;

        data.append(runCount);
        data.append(highByte);
        data.append(lowByte);

        header.append(QString("  %1, 0x%2, 0x%3,\n")
                      .arg(runCount, 3)
                      .arg(highByte, 2, 16, QLatin1Char('0'))
                      .arg(lowByte,  2, 16, QLatin1Char('0')));
    }

    header.append("};\n\n");
    header.append(QString("Animation animation(%1, animationData, ENCODING_RGB565_RLE, %2);\n")
                  .arg(image.width())
                  .arg(image.height()));

//    for(int i = 0; i < data.length()/3; i++) {
//        qDebug() << "RunLength:" << (int)data[i*3]
//                 << "Color:" << (int)data[i*3+1] << (int)data[i*3+2];
//    }
}


void Animation::encodeImageRGB24() {
    data.clear();

    for(int frame = 0; frame < image.width(); frame++) {
        for(int pixel = 0; pixel < image.height(); pixel++) {
            QRgb color = ColorModel::correctBrightness(image.pixel(frame, pixel));
            data.append(qRed(color));
            data.append(qGreen(color));
            data.append(qBlue(color));
        }
    }
}
