#ifndef PATTERNUPLOADER_H
#define PATTERNUPLOADER_H

#include <QObject>
#include "pattern.h"
#include "blinkytape.h"

/// This is an re-entreant version of an pattern uploader.
/// Each task in the upload process is broken into a single state, and the state
/// machine is driven forward by receiving events from the connected serial port,
/// or in the case of a timeout, from the timeout timer.
///
/// This seems convoluted, but the goal is to avoid ever waiting on a serial read
/// event. For some reason, doing so seems to block the main thread even if we are
/// in a different thread; it's possible serial sits on the main lock?
///
/// While the upload process is underway, it will send periodic progress updates
/// via the progressUpdate() signal.
class PatternUploader : public QObject
{
    Q_OBJECT

public:
    PatternUploader(QObject *parent=0) :
        QObject(parent) {}

    /// Start an upload, using the passed blinkytape as a launching point
    /// Note that the blinkytape will be disconnected during the upload process,
    /// and will need to be reconnected manually afterwards.
    /// @param tape BlinkyTape to upload to (must already be connected)
    /// @param pattern Pattern to upload to the BlinkyTape
    virtual bool startUpload(BlinkyTape& tape, std::vector<Pattern> patterns) = 0;

    /// Start an upload, using the passed blinkytape as a launching point
    /// Note that the blinkytape will be disconnected during the upload process,
    /// and will need to be reconnected manually afterwards.
    /// @param tape BlinkyTape to upload to (must already be connected)
    /// @param sketch Sketch to upload to the BlinkyTape
    virtual bool startUpload(BlinkyTape& tape, QByteArray sketch) = 0;

    /// Get a string describing the last error, if any.
    virtual QString getErrorString() const = 0;

signals:
    /// Sends an update about the maximum update progress, from 0 to 1
    void maxProgressChanged(int progress);

    /// Sends an update about the upload progress, from 0 to 1
    void progressChanged(int progress);

    /// Sends a signal at end of upload to report the result.
    void finished(bool result);
};

#endif // PATTERNUPLOADER_H

