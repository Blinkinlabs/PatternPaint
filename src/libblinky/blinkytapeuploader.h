#ifndef AVRPATTERNUPLOADER_H
#define AVRPATTERNUPLOADER_H

#include "patternwriter.h"
#include "blinkyuploader.h"
#include "serialcommandqueue.h"
#include "memorysection.h"

#include <QObject>
#include <QTimer>
#include <iostream>

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
class BlinkyTapeUploader : public BlinkyUploader
{
    Q_OBJECT

public:
    BlinkyTapeUploader(QObject *parent = 0);

    bool storePatterns(BlinkyController &blinky, QList<PatternWriter> &patternWriters);
    bool updateFirmware(BlinkyController &blinky);
    bool restoreFirmware(qint64 timeout);
    QString getErrorString() const;

    QList<PatternWriter::Encoding> getSupportedEncodings() const;

public slots:
    void cancel();

private slots:
    void doWork();  /// Handle the next section of work, whatever it is

    void handleError(QString error);

    void handleCommandFinished(QString command, QByteArray returnData);

    void handleLastCommandFinished();

private:
    enum State {
        State_ProbeBootloaders,         ///< Waiting for the bootloader device to become available
        State_InitializeBootloader,     ///< Sending initialization commands to bootloader
        State_WriteFlashData,           ///< Writing flash data sections
        State_ResetBootloader,          ///< Resetting bootloader
        State_SendingCommands,          ///< Ready for a command. TODO: Delete me
    };

    /// Start an upload, using the passed blinkytape as a launching point
    /// Note that the blinkytape will be disconnected during the upload process,
    /// and will need to be reconnected manually afterwards.
    bool startUpload(BlinkyController &tape);

    bool startUpload(qint64 timeout);

    /// Update any listeners with the latest progress
    void setProgress(int newProgress);

    /// Update text in upload window
    void setDialogText();

    /// True if a bootloader device is available to connect to
    bool bootloaderAvailable();

    bool ProbeBootloaderTimeout();


    /// Variables used in all states

    int progress;           ///< Current upload progress, in command counts
    int maxProgress;        ///< Total expected progress states
    QString textLabel;      ///< text in upload window

    QString errorString;    ///< Error string for asyncronous debugging

    State state;            ///< Current command state


    qint64 probeBootloaderTimeout;       ///< Amount of time to wait for a bootloader device to appear
    QDateTime probeBootloaderStartTime;  ///< Time that we transitioned into the current state

    int flashWriteRetriesRemaining;      ///< Number of times we can re-try writing the flash before giving up

    SerialCommandQueue commandQueue;
    QList<MemorySection> flashData;     ///< Queue of memory sections to write
};

#endif // AVRPATTERNUPLOADER_H
