#ifndef PATTERNUPLOADER_H
#define PATTERNUPLOADER_H

#include <QObject>
#include <QTimer>
#include <iostream>
#include "pattern.h"
#include "avrprogrammer.h"
#include "blinkytape.h"

struct FlashSection {
    FlashSection(int address,
                 QByteArray data) :
        address(address),
        data(data) {}

    int address;
    QByteArray data;
};

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
    PatternUploader(QObject *parent=0);

    /// Start an upload, using the passed blinkytape as a launching point
    /// Note that the blinkytape will be disconnected during the upload process,
    /// and will need to be reconnected manually afterwards.
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

signals:
    /// Sends an update about the upload progress, from 0 to 1
    void progressChanged(int progress);

    /// Sends a signal at end of upload to report the result.
    void finished(bool result);

private slots:
    void doWork();  /// Handle the next section of work, whatever it is

    void handleProgrammerError(QString error);

    void handleProgrammerCommandFinished(QString command, QByteArray returnData);

    /// Delay timer, lets us wait some time between receiving a finished command, and
    /// passing the message along (to give the serial device some time to reset itself).
    void handleResetTimer();

private:
    enum State {
        State_Ready,                    ///< Ready for a command.
        State_WaitForBootloaderPort,    ///< We are waiting for the bootloader port to show up.
        State_WaitForDeviceSignature,   ///< Waiting to hear the device signature
        State_WaitForSketchWritten,     ///< Waiting for the sketch to be written
        State_WaitForMetaDataWritten,   ///<
        State_Finished,                 ///< Done.
    };

    /// Timer that fires
    QTimer *processTimer;

    float progress;

    QString errorString;

    /// Time that we transitioned into the current state
    QDateTime stateStartTime;

    /// Current command state
    State state;

    /// Update any listeners with the latest progress
    void updateProgress(int newProgress);

    AvrProgrammer programmer;

    QQueue<FlashSection> flashData; ///< Queue of memory sections to write

    /// TODO: Delete me?
    bool waitOneMore; ///< BS variable to delay connection to bootloader by 1 timer cycle
};

#endif // PATTERNUPLOADER_H
