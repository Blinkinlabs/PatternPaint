#ifndef AVRPATTERNUPLOADER_H
#define AVRPATTERNUPLOADER_H

#include "patternwriter.h"
#include "blinkyuploader.h"
#include "serialcommandqueue.h"

#include <QObject>
#include <QTimer>
#include <iostream>

struct FlashSection {
    /// Create a new flash section
    /// @param address Address in the flash memory where the data should be stored
    /// @param data Data to store
    FlashSection(int address, QByteArray data) :
        address(address),
        data(data)
    {
    }

    /// Address to store the data
    int address;

    /// Data to store
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
class BlinkyTapeUploader : public BlinkyUploader
{
    Q_OBJECT

public:
    BlinkyTapeUploader(QObject *parent = 0);

    bool startUpload(BlinkyController &blinky, QList<PatternWriter> &patternWriters);
    bool upgradeFirmware(BlinkyController &blinky);
    bool upgradeFirmware(int timeout);
    QString getErrorString() const;

    QList<PatternWriter::Encoding> getSupportedEncodings() const;

public slots:
    void cancel();

private slots:
    void doWork();  /// Handle the next section of work, whatever it is

    void handleError(QString error);

    void handleCommandFinished(QString command, QByteArray returnData);

private:
    enum State {
        State_Ready,                    ///< Ready for a command.
        State_WaitForBootloaderPort,    ///< We are waiting for the bootloader device to show up.
        State_WaitAfterBootloaderPort,  ///< Short delay after the device shows up
    };

    /// Start an upload, using the passed blinkytape as a launching point
    /// Note that the blinkytape will be disconnected during the upload process,
    /// and will need to be reconnected manually afterwards.
    bool startUpload(BlinkyController &tape);

    qint64 bootloaderPollTimeout;

    /// Current upload progress, in command counts
    int progress;
    int maxProgress;

    QString errorString;

    /// Time that we transitioned into the current state
    QDateTime stateStartTime;

    /// Current command state
    State state;

    /// Update any listeners with the latest progress
    void setProgress(int newProgress);

    SerialCommandQueue commandQueue;

    QQueue<FlashSection> flashData; ///< Queue of memory sections to write
};

#endif // AVRPATTERNUPLOADER_H
