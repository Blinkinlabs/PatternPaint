#ifndef BLINKYPENDANTUPLOADDATA_H
#define BLINKYPENDANTUPLOADDATA_H

#include <QByteArray>
#include <QDebug>
#include "patternwriter.h"
#include "memorysection.h"

/// Utility class for assembling flash sections for the BlinkyPendant
class BlinkyPendantUploadData
{
public:
    enum DisplayMode {
        POV = 10,
        TIMED = 11,
    };

    static QByteArray makePatternTableHeader(uint8_t patternCount, DisplayMode displayMode);
    static QByteArray makePatternTableEntry(uint32_t offset,
                                            uint16_t frameCount,
                                            uint16_t frameDelay);

    bool init(DisplayMode displayMode,
              const QList<PatternWriter> &patternWriters);

    QByteArray data;

    QString errorString;
};

#endif // BLINKYPENDANTUPLOADDATA_H
