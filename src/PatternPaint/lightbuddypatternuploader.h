#ifndef LIGHTBUDDYPATTERNUPLOADER_H
#define LIGHTBUDDYPATTERNUPLOADER_H

#include "patternuploader.h"
#include "blinkytape.h"

// TODO: Don't call it a blinkytape??

class LightBuddyPatternUploader : public PatternUploader
{
public:
    LightBuddyPatternUploader();
    ~LightBuddyPatternUploader();

    /// Start an upload, using the passed blinkytape as a launching point
    /// @param tape BlinkyTape to upload to (must already be connected)
    /// @param pattern Pattern to upload to the BlinkyTape
    bool startUpload(BlinkyTape& tape, std::vector<Pattern> patterns);

    /// Start an upload, using the passed blinkytape as a launching point
    /// Note that the blinkytape will be disconnected during the upload process,
    /// and will need to be reconnected manually afterwards.
    /// @param tape BlinkyTape to upload to (must already be connected)
    /// @param sketch Sketch to upload to the BlinkyTape
    bool startUpload(BlinkyTape& tape, QByteArray sketch);

    /// Get a string describing the last error, if any.
    QString getErrorString() const;
};

#endif // LIGHTBUDDYPATTERNUPLOADER_H
