#ifndef BLINKYPENDANTPATTERNUPLOADER_H
#define BLINKYPENDANTPATTERNUPLOADER_H

#include "patternuploader.h"
#include "blinkytape.h"

class BlinkyPendantPatternUploader : public PatternUploader
{
public:
    BlinkyPendantPatternUploader(QObject *parent);

    bool startUpload(BlinkyController& tape, std::vector<Pattern> patterns);
    bool upgradeFirmware(BlinkyController& tape);
    QString getErrorString() const;

private:
    QString errorString;
};

#endif // BLINKYPENDANTPATTERNUPLOADER_H
