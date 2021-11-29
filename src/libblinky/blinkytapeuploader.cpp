#include "blinkytapeuploader.h"

#include "blinkytapeuploaddata.h"
#include "firmwarestore.h"

#include <QDebug>

BlinkyTapeUploader::BlinkyTapeUploader(QObject *parent) :
    BlinkyUploader(parent),
    firmwareLoader(this)
{
    connect(&firmwareLoader, &FirmwareLoader::progressChanged,
            this, &BlinkyTapeUploader::progressChanged);
    connect(&firmwareLoader, &FirmwareLoader::setText,
            this, &BlinkyTapeUploader::setText);
    connect(&firmwareLoader, &FirmwareLoader::finished,
            this, &BlinkyTapeUploader::finished);
}

QList<PatternWriter::Encoding> BlinkyTapeUploader::getSupportedEncodings() const
{
    QList<PatternWriter::Encoding> encodings;
    encodings.append(PatternWriter::RGB565_RLE);
    encodings.append(PatternWriter::RGB24);
    return encodings;
}

void BlinkyTapeUploader::cancel()
{
    firmwareLoader.cancel();
}

bool BlinkyTapeUploader::storePatterns(BlinkyController &blinky, QList<PatternWriter> &patternWriters)
{
    /// Create the compressed image and check if it will fit into the device memory
    BlinkyTapeUploadData data;

    // TODO: Get this from the current scene rather than from preferences
    QSettings settings;
    QString firmwareName = settings.value("BlinkyTape/firmwareName", BLINKYTAPE_DEFAULT_FIRMWARE_NAME).toString();

    if (!data.init(firmwareName, patternWriters)) {
        errorString = data.errorString;
        return false;
    }

    return firmwareLoader.updateFirmware(blinky, data.flashData, data.eepromData);
}


QString BlinkyTapeUploader::getErrorString() const
{
    if(!errorString.isEmpty())
        return errorString;

    return firmwareLoader.getErrorString();
}
