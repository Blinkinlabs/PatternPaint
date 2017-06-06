#ifndef EIGHTBYEIGHTUPLOADER_H
#define EIGHTBYEIGHTUPLOADER_H

#include "libblinkyglobal.h"

#include "blinkyuploader.h"
#include "serialcommandqueue.h"

#include <QObject>

class LIBBLINKY_EXPORT EightByEightUploader : public BlinkyUploader
{
    Q_OBJECT

public:
    EightByEightUploader(QObject *parent = 0);

    bool storePatterns(BlinkyController &controller, QList<PatternWriter> &patternWriters);
    QString getErrorString() const;

    QList<PatternWriter::Encoding> getSupportedEncodings() const;

public slots:
    void cancel();

private slots:
    void doWork();  /// Handle the next section of work, whatever it is

    void handleError(QString error);

    void handleCommandFinished(QString command, QByteArray returnData);

    void handleCommandStillRunning(QString command);

    void handleLastCommandFinished();

private:
    /// Update any listeners with the latest progress
    void setProgress(int newProgress);

    QString errorString;

    SerialCommandQueue commandQueue;

    enum State {
        State_checkFirmwareVersion,///< Check version
        State_lockFileAccess,      ///< Lock file access
        State_erasePatterns,       ///< Erase the patterns
        State_WriteFile,           ///< Write files
        State_unlockFileAccess,    ///< Unlock file access
        State_Done                 ///< And we're done!
    };

    State state;
    int patternIndex;

    int progress;
    int maxProgress;

    int sector; /// Current file sector that is being written to/read from

    QQueue<QByteArray> flashData; ///< Queue of memory sections to write
};

#endif // EIGHTBYEIGHTUPLOADER_H
