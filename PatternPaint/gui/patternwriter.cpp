#include "patternwriter.h"

#include "colormode.h"

#include <QDebug>

PatternWriter::PatternWriter(const Pattern *pattern, Encoding encoding, Fixture *fixture) :
    encoding(encoding),
    fixture(fixture)
{
    frameCount = pattern->getFrameCount();
    frameDelay = 1000/pattern->getFrameSpeed();
    ledCount = fixture->getLedCount();

    // Create a new encoder
    switch (encoding) {
    case RGB565_RLE:
        encodeImageRGB565_RLE(pattern);
        break;
    case RGB24:
        encodeImageRGB24(pattern);
        break;
    }
}

PatternWriter::Encoding PatternWriter::getEncoding() const
{
    return encoding;
}

QByteArray& PatternWriter::getData()
{
    return data;
}

QString PatternWriter::getHeader() const
{
    return header;
}

int PatternWriter::getFrameCount() const
{
    return frameCount;
}

int PatternWriter::getLedCount() const
{
    return ledCount;
}

int PatternWriter::getFrameDelay() const
{
    return frameDelay;
}

int PatternWriter::colorCount() const
{
    // TODO
    QImage image;

    // Brute force method for counting the number of unique colors in the image
    QList<QRgb> colors;

    for (int frame = 0; frame < image.width(); frame++) {
        for (int pixel = 0; pixel < image.height(); pixel++) {
            QRgb color = image.pixel(frame, pixel);
            if (!colors.contains(color))
                colors.append(color);
        }
    }

    return colors.length();
}

int PatternWriter::QRgbTo565(QColor color)
{
    switch (fixture->getColorMode()) {
    case GRB:
        return (((color.green()   >> 3) & 0x1F) << 11)
               | (((color.red()   >> 2) & 0x3F) <<  5)
               | (((color.blue()  >> 3) & 0x1F));
        break;
    case RGB:
    default:
        return (((color.red()     >> 3) & 0x1F) << 11)
               | (((color.green() >> 2) & 0x3F) <<  5)
               | (((color.blue()  >> 3) & 0x1F));
        break;
    }
}

void PatternWriter::encodeImageRGB565_RLE(const Pattern *pattern)
{
//    qDebug() << "Encoding pattern as RGB565";

    data.clear();
    header.clear(); // TODO: Move the header builder somewhere else?

    header.append("const uint8_t animationData[] PROGMEM = {\n");

    for (int frame = 0; frame < pattern->getFrameCount(); frame++) {
        header.append(QString("// Frame: %1\n").arg(frame));

        QList<QColor> colorStream = fixture->getColorStreamForFrame(pattern->getFrameImage(frame));

        int currentColor = 0;
        int runCount = 0;

        for (int pixel = 0; pixel < colorStream.count(); pixel++) {
            QColor color = colorStream.at(pixel);

            int decimatedColor = QRgbTo565(color);

            if (runCount == 0)
                currentColor = decimatedColor;

            if ((currentColor != decimatedColor) || (runCount == 255)) {
                int highByte = (currentColor >> 8) & 0xFF;
                int lowByte = (currentColor)      & 0xFF;

                data.append(runCount);
                data.append(highByte);
                data.append(lowByte);

                header.append(QString("  %1, 0x%2, 0x%3,\n")
                              .arg(runCount, 3)
                              .arg(highByte, 2, 16, QLatin1Char('0'))
                              .arg(lowByte, 2, 16, QLatin1Char('0')));

                runCount = 1;
                currentColor = decimatedColor;
            } else {
                runCount++;
            }
        }

        int highByte = (currentColor >> 8) & 0xFF;
        int lowByte = (currentColor)      & 0xFF;

        data.append(runCount);
        data.append(highByte);
        data.append(lowByte);

        header.append(QString("  %1, 0x%2, 0x%3,\n")
                      .arg(runCount, 3)
                      .arg(highByte, 2, 16, QLatin1Char('0'))
                      .arg(lowByte, 2, 16, QLatin1Char('0')));
    }

    header.append("};\n\n");
    header.append(QString("Animation animation(%1, animationData, Animation::RGB565_RLE, %2, %3);\n")
                  .arg(pattern->getFrameCount())
                  .arg(fixture->getLedCount())
                  .arg(frameDelay));
}

void PatternWriter::encodeImageRGB24(const Pattern *pattern)
{
//    qDebug() << "Encoding pattern as RGB24";

    header.clear();
    data.clear();

    for (int frame = 0; frame < pattern->getFrameCount(); frame++) {
        QList<QColor> colorStream = fixture->getColorStreamForFrame(pattern->getFrameImage(frame));

        for (int pixel = 0; pixel < colorStream.count(); pixel++) {
            QColor color = colorStream.at(pixel);

            switch (fixture->getColorMode()) {
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

    for (int frame = 0; frame < pattern->getFrameCount(); frame++) {
        header.append(QString("// Frame: %1\n").arg(frame));

        QList<QColor> colorStream = fixture->getColorStreamForFrame(pattern->getFrameImage(frame));

        for (int pixel = 0; pixel < colorStream.count(); pixel++) {
            QColor color = colorStream.at(pixel);

            switch (fixture->getColorMode()) {
            case GRB:
                header.append(QString("    %1, %2, %3, // %4\n")
                              .arg(color.green(), 3)
                              .arg(color.red(), 3)
                              .arg(color.blue(), 3)
                              .arg(pixel));
                break;
            case RGB:
            default:
                header.append(QString("    %1, %2, %3, // %4\n")
                              .arg(color.red(), 3)
                              .arg(color.green(), 3)
                              .arg(color.blue(), 3)
                              .arg(pixel));
                break;
            }
        }
    }

    header.append("};\n");
    header.append("\n");
    header.append(QString("Animation animation(%1, animationData, Animation::RGB24, %2, %3);")
                  .arg(pattern->getFrameCount())
                  .arg(fixture->getLedCount())
                  .arg(frameDelay));
}
