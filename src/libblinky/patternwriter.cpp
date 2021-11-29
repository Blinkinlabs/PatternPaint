#include "patternwriter.h"

#include "colormode.h"

#include <QDebug>

PatternWriter::PatternWriter(const Pattern &pattern, const Fixture &fixture, Encoding encoding) :
    encoding(encoding)
{
    frameCount = pattern.getFrameCount();
    frameDelay = 1000/pattern.getFrameSpeed();
    ledCount = fixture.getCount();
    repeatCount = pattern.getPatternRepeatCount();

    // Create a new encoder
    switch (encoding) {
    case RGB565_RLE:
        encodeImageRGB565_RLE(pattern, fixture);
        break;
    case RGB24:
        encodeImageRGB24(pattern, fixture);
        break;
    }
}

PatternWriter::Encoding PatternWriter::getEncoding() const
{
    return encoding;
}

const QByteArray & PatternWriter::getDataAsBinary() const
{
    return data;
}

const QString & PatternWriter::getDataAsHeader() const
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

int PatternWriter::getRepeatCount() const
{
    return repeatCount;
}

int PatternWriter::QRgbTo565(const QColor &color, ColorMode colorMode)
{
    QByteArray bytes = colorToBytes(colorMode, color);

    // TODO: is the cast necessicary here?
    return   (((static_cast<uint8_t>(bytes.at(0)) >> 3) & 0x1F) << 11)
           | (((static_cast<uint8_t>(bytes.at(1)) >> 2) & 0x3F) <<  5)
           | (((static_cast<uint8_t>(bytes.at(2)) >> 3) & 0x1F));
}

void PatternWriter::encodeImageRGB565_RLE(const Pattern &pattern, const Fixture &fixture)
{
    data.clear();
    header.clear(); // TODO: Move the header builder somewhere else?

    header.append("const uint8_t animationData[] PROGMEM = {\n");

    for (int frame = 0; frame < pattern.getFrameCount(); frame++) {
        header.append(QString("// Frame: %1\n").arg(frame));

        // TODO: Is the brightness correction being applied correctly here?
        QList<QColor> colorStream = fixture.getColorStream(pattern.getFrameImage(frame));

        int currentColor = 0;
        int runCount = 0;

        for (int pixel = 0; pixel < colorStream.count(); pixel++) {
            QColor color = colorStream.at(pixel);

            int decimatedColor = QRgbTo565(color, fixture.getColorMode());

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
                  .arg(pattern.getFrameCount())
                  .arg(fixture.getCount())
                  .arg(frameDelay));
}

void PatternWriter::encodeImageRGB24(const Pattern &pattern, const Fixture &fixture)
{
    header.clear();
    data.clear();

    if(!colorModeValid(fixture.getColorMode()))
          return;

    for (int frame = 0; frame < pattern.getFrameCount(); frame++) {
        QList<QColor> colorStream = fixture.getColorStream(pattern.getFrameImage(frame));

        for (QColor color : colorStream)
            data.append(colorToBytes(fixture.getColorMode(), color));
    }

    header.append("const uint8_t animationData[] PROGMEM = {\n");

    for (int frame = 0; frame < pattern.getFrameCount(); frame++) {
        header.append(QString("// Frame: %1\n").arg(frame));

        QList<QColor> colorStream = fixture.getColorStream(pattern.getFrameImage(frame));

        for (int pixel = 0; pixel < colorStream.count(); pixel++) {
            QByteArray bytes = colorToBytes(fixture.getColorMode(), colorStream.at(pixel));
            header.append(QString("    %1, %2, %3, // %4\n")
                          .arg(static_cast<uint8_t>(bytes.at(0)), 3)
                          .arg(static_cast<uint8_t>(bytes.at(1)), 3)
                          .arg(static_cast<uint8_t>(bytes.at(2)), 3)
                          .arg(pixel));
        }
    }

    header.append("};\n");
    header.append("\n");
    header.append(QString("Animation animation(%1, animationData, Animation::RGB24, %2, %3);")
                  .arg(pattern.getFrameCount())
                  .arg(fixture.getCount())
                  .arg(frameDelay));
}
