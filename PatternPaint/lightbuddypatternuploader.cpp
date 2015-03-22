#include "lightbuddypatternuploader.h"

LightBuddyPatternUploader::LightBuddyPatternUploader()
{

}

LightBuddyPatternUploader::~LightBuddyPatternUploader()
{

}

bool LightBuddyPatternUploader::startUpload(BlinkyTape &tape, std::vector<Pattern> patterns)
{
    Q_UNUSED(tape);
    Q_UNUSED(patterns);

    // Upload procedure:
    // 1. Count the number of stored patterns;
    // 2. If we have 6, iterate through sectors until the last file is found
    // 3. If we have 6, delete the last file
    // 4. Create a new file to store the pattern
    // 5. Upload each sector to the lightbuddy
    // 6. Reset the lightbuddy state machine
    return false;
}

bool LightBuddyPatternUploader::startUpload(BlinkyTape &tape, QByteArray sketch)
{
    Q_UNUSED(tape);
    Q_UNUSED(sketch);

    // TODO: Support firmware upload for the lightbuddy
    return false;
}

QString LightBuddyPatternUploader::getErrorString() const
{
    return "";
}

