#include "patternoutput.h"
#include "colormodel.h"

#include <QDebug>

PatternOutput::PatternOutput(QImage image, int frameDelay, Encoding encoding, ColorMode colorMode) :
    encoding(encoding),
    image(image),
    frameDelay(frameDelay),
    colorMode(colorMode)
{
    frameCount = image.width();
    ledCount = image.height();

    // Create a new encoder
    switch(encoding) {
    case RGB565_RLE:
        encodeImageRGB16_RLE();
        break;
    case RGB24:
        encodeImageRGB24();
        break;
    case INDEXED:
        encodeImageIndexed();
        break;
    case INDEXED_RLE:
        encodeImageIndexed_RLE();
        break;
    }
}

int PatternOutput::colorCount() const
{
    // Brute force method for counting the number of unique colors in the image
    QList<QRgb> colors;

    for(int frame = 0; frame < image.width(); frame++) {
        for(int pixel = 0; pixel < image.height(); pixel++) {
            QRgb color = image.pixel(frame, pixel);
            if(!colors.contains(color)) {
                colors.append(color);
            }
        }
    }

    return colors.length();
}

int PatternOutput::QRgbTo565(QColor color) {
    switch(colorMode) {
        case GRB:
            return (((color.green()   >> 3) & 0x1F)   << 11)
                   | (((color.red()   >> 2) & 0x3F)   <<  5)
                   | (((color.blue()  >> 3) & 0x1F)        );
            break;
        case RGB:
        default:
            return (((color.red()     >> 3) & 0x1F)   << 11)
                   | (((color.green() >> 2) & 0x3F)   <<  5)
                   | (((color.blue()  >> 3) & 0x1F)        );
            break;
    }
}

void PatternOutput::encodeImageRGB16_RLE() {
    // TODO: Do I work?

    data.clear();
    header.clear(); // TODO: Move the header builder somewhere else?

    header.append("const uint8_t animationData[] PROGMEM = {\n");

    for(int frame = 0; frame < image.width(); frame++) {
        int currentColor;
        int runCount = 0;

        for(int pixel = 0; pixel < image.height(); pixel++) {
            QColor color = ColorModel::correctBrightness(image.pixel(frame, pixel));

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
}


void PatternOutput::encodeImageRGB24() {
    // TODO: build c++ header as well

    header.clear();
    data.clear();

    for(int frame = 0; frame < image.width(); frame++) {
        for(int pixel = 0; pixel < image.height(); pixel++) {
            QColor color = ColorModel::correctBrightness(image.pixel(frame, pixel));

            switch(colorMode) {
                case GRB:
                    data.append(color.green());
                    data.append(color.red());
                    data.append(color.blue());
                    break;
                case RGB:
                default:
                    data.append(color.red());
                    data.append(color.green());
                    data.append(color.blue());
                    break;
            }
        }
    }

    header.append("const uint8_t animationData[] PROGMEM = {\n");

    for(int frame = 0; frame < image.width(); frame++) {
        for(int pixel = 0; pixel < image.height(); pixel++) {
            QColor color = ColorModel::correctBrightness(image.pixel(frame, pixel));

            switch(colorMode) {
                case GRB:
                    header.append(QString("    %1, %2, %3, // %4\n")
                                  .arg(color.green())
                                  .arg(color.red())
                                  .arg(color.blue())
                                  .arg(pixel));
                    break;
                case RGB:
                default:
                header.append(QString("    %1, %2, %3, // %4\n")
                              .arg(color.red())
                              .arg(color.green())
                              .arg(color.blue())
                              .arg(pixel));
                   break;
            }


        }
    }

    header.append("};\n");
    header.append("\n");
    header.append(QString("Animation animation(%1, animationData, ENCODING_RGB24, %2);")
                          .arg(image.width())
                          .arg(image.height()));
}

void PatternOutput::encodeImageIndexed() {
    // TODO: Add color mode support

    header.clear();
    data.clear();

    // First, convert to an indexed format. This should be non-destructive
    // if the image had fewer than 256 colors.
    QImage indexed = image.convertToFormat(
                QImage::Format_Indexed8,
                Qt::AutoColor & Qt::AvoidDither);
    qDebug() << "Original color count:" << colorCount();
    qDebug() << "Indexed color count: " << indexed.colorCount();

    header.append("const PROGMEM prog_uint8_t patternData[]  = {\n");

    // Record the length of the color table
    header.append("// Length of the color table - 1, in bytes. length: 1 byte\n");
    header.append(QString(" %1,\n")
                  .arg(indexed.colorCount() - 1, 3));

    data.append(indexed.colorCount() - 1);

    // Build the color table
    header.append(QString("// Color table section. Each entry is 3 bytes. length: %1 bytes\n")
                  .arg(indexed.colorCount()*3));

    for (int index = 0; index < indexed.colorCount(); index++) {
        QColor color = ColorModel::correctBrightness(indexed.color(index));

        header.append(QString(" %1, %2, %3,\n")
                          .arg(color.red(),   3)
                          .arg(color.green(), 3)
                          .arg(color.blue(),  3));

        /// Colors in the color table are stored in RGB24 format
        data.append(color.red());
        data.append(color.green());
        data.append(color.blue());
    }


    // Build the pixel table
    header.append(QString("// Pixel table section. Each pixel is 1 byte. length: %1 bytes\n")
                  .arg(image.width()*image.height()));

    for(int frame = 0; frame < image.width(); frame++) {
        for(int pixel = 0; pixel < image.height(); pixel++) {
            int index = indexed.pixelIndex(frame, pixel);
            header.append(QString(" %1,").arg(index, 3));

            if (((pixel + frame*image.height()) % 10 == 9)) {
                header.append("\n");
            }

            /// Pixel indexes are stired as 8-bit indexes
            data.append(index);
        }
    }

    header.append("\n};\n\n");
    header.append(QString("Pattern pattern(%1, patternData, ENCODING_INDEXED, %2);\n")
                  .arg(image.width())
                  .arg(image.height()));

    qDebug() << "Pattern size:" << data.length();
}

void PatternOutput::encodeImageIndexed_RLE() {
    // TODO: Add color mode support

    header.clear();
    data.clear();

    // First, convert to an indexed format. This should be non-destructive
    // if the image had fewer than 256 colors.
    QImage indexed = image.convertToFormat(
                QImage::Format_Indexed8,
                Qt::AutoColor & Qt::AvoidDither);
    qDebug() << "Original color count:" << colorCount();
    qDebug() << "Indexed color count: " << indexed.colorCount();

    header.append("const PROGMEM prog_uint8_t patternData[]  = {\n");

    // Record the length of the color table
    header.append("// Length of the color table - 1, in bytes. length: 1 byte\n");
    header.append(QString(" %1,\n")
                  .arg(indexed.colorCount() - 1, 3));

    data.append(indexed.colorCount() - 1);

    // Build the color table
    header.append(QString("// Color table section. Each entry is 3 bytes. length: %1 bytes\n")
                  .arg(indexed.colorCount()*3));

    for (int index = 0; index < indexed.colorCount(); index++) {
        // TODO: Brightness correction before pallete reduction?
        QColor color = ColorModel::correctBrightness(indexed.color(index));

        header.append(QString(" %1, %2, %3,\n")
                          .arg(color.red(),   3)
                          .arg(color.green(), 3)
                          .arg(color.blue(),  3));

        /// Colors in the color table are stored in RGB24 format
        data.append(color.red());
        data.append(color.green());
        data.append(color.blue());
    }


    // Build the pixel table
    header.append(QString("// Pixel runs section. Each pixel run is 2 bytes. length: %1 bytes\n")
                  .arg(-1));

    for(int frame = 0; frame < image.width(); frame++) {
        int currentColor;
        int runCount = 0;

        for(int pixel = 0; pixel < image.height(); pixel++) {
            int newColor = indexed.pixelIndex(frame, pixel);

            if(runCount == 0) {
                currentColor = newColor;
            }

            if(currentColor != newColor) {
                header.append(QString(" %1, %2,\n")
                              .arg(runCount, 3)
                              .arg(currentColor, 3));

                data.append(runCount);
                data.append(currentColor);

                runCount = 1;
                currentColor = newColor;
            }
            else {
                runCount++;
            }
        }

        header.append(QString(" %1, %2,\n")
                      .arg(runCount, 3)
                      .arg(currentColor, 3));

        data.append(runCount);
        data.append(currentColor);
    }

    header.append("\n};\n\n");
    header.append(QString("Pattern pattern(%1, patternData, ENCODING_INDEXED_RLE, %2);\n")
                  .arg(image.width())
                  .arg(image.height()));

    qDebug() << "Pattern size:" << data.length();
}

