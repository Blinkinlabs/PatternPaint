#include "blinkypendantuploader.h"

BlinkyPendantPatternUploader::BlinkyPendantPatternUploader(QObject *parent) :
    PatternUploader(parent)
{

}

bool BlinkyPendantPatternUploader::startUpload(BlinkyController& tape, std::vector<Pattern> patterns)
{
    Q_UNUSED(tape);
    Q_UNUSED(patterns);

    errorString = "Pattern upload not currently supported for BlinkyPendant!";
    return false;
}

bool BlinkyPendantPatternUploader::upgradeFirmware(BlinkyController& tape)
{
    Q_UNUSED(tape);

    errorString = "Firmware update not currently supported for BlinkyPendant!";
    return false;
}

QString BlinkyPendantPatternUploader::getErrorString() const
{
    return errorString;
}
