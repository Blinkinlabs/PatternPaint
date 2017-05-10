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
    static QByteArray makePatternTableHeader(uint8_t patternCount, uint8_t displayMode);
    static QByteArray makePatternTableEntry(uint32_t offset,
                                            uint16_t frameCount,
                                            uint16_t frameDelay);

    bool init(const QList<PatternWriter> &patternWriters);

    QByteArray data;

    QString errorString;
};

#endif // BLINKYPENDANTUPLOADDATA_H
