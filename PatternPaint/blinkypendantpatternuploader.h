#ifndef BLINKYPENDANTPATTERNUPLOADER_H
#define BLINKYPENDANTPATTERNUPLOADER_H

#include "patternuploader.h"
#include "blinkytape.h"

class BlinkyPendantPatternUploader : public PatternUploader
{
public:
    BlinkyPendantPatternUploader();
    ~BlinkyPendantPatternUploader();

    bool startUpload(BlinkyTape& tape, std::vector<Pattern> patterns);
    bool upgradeFirmware(BlinkyTape& tape);
    QString getErrorString() const;

private:
    QString errorString;
};

#endif // BLINKYPENDANTPATTERNUPLOADER_H
