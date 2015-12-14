#ifndef BLINKYUPLOADER_H
#define BLINKYUPLOADER_H

#include "patternwriter.h"

#include <QObject>
#include <QList>

class BlinkyController;

/// This is an re-entreant version of an pattern uploader.
/// Each task in the upload process is broken into a single state, and the state
/// machine is driven forward by receiving events from the connected serial port,
/// or in the case of a timeout, from the timeout timer.
///
/// This seems convoluted, but the goal is to avoid ever waiting on a serial read
/// event. For some reason, doing so seems to block the main thread even if we are
/// in a different thread; it's possible serial sits on a main lock?
///
/// While the upload process is underway, it will send periodic progress updates
/// via the progressUpdate() signal.
class BlinkyUploader : public QObject
{
    Q_OBJECT

public:
    BlinkyUploader(QObject *parent = 0) :
        QObject(parent)
    {
    }

    /// Start an upload, using the passed blinkytape as a launching point
    /// Note that the blinkytape will be disconnected during the upload process,
    /// and will need to be reconnected manually afterwards.
    /// @param tape BlinkyTape to upload to (must already be connected)
    /// @param pattern Pattern to upload to the BlinkyTape
    virtual bool startUpload(BlinkyController &controller, std::vector<PatternWriter> patterns) = 0;

    /// Update/restore the firmware on the specificed device
    /// @param tape BlinkyTape to upload to (must already be connected)
    virtual bool upgradeFirmware(BlinkyController &controller) = 0;

    /// Scan for a device bootloader, then attempt to restore the firmware to it.
    /// @param timeout Number of seconds to wait for a bootloader, or -1 to wait
    /// indefinetly
    virtual bool upgradeFirmware(int timeout) = 0;

    /// Get a string describing the last error, if any.
    virtual QString getErrorString() const = 0;

    virtual QList<PatternWriter::Encoding> getSupportedEncodings() const = 0;

signals:
    /// Sends an update about the upload progress, from 0 to 1
    void progressChanged(float progress);

    /// Sends a signal at end of upload to report the result.
    void finished(bool result);

public slots:
    /// Request that the upload process be stopped
    virtual void cancel() = 0;
};

#endif // BLINKYUPLOADER_H
