#ifndef EIGHTBYEIGHTUPLOADER_H
#define EIGHTBYEIGHTUPLOADER_H

#include "blinkyuploader.h"
#include "serialcommandqueue.h"

#include <QObject>

class EightByEightUploader : public BlinkyUploader
{
    Q_OBJECT

public:
    EightByEightUploader(QObject *parent = 0);

    bool storePatterns(BlinkyController &controller, QList<PatternWriter> &patternWriters);
    bool updateFirmware(BlinkyController &controller);
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
    QString errorString;

    SerialCommandQueue commandQueue;

    /// Update any listeners with the latest progress
    void setProgress(int newProgress);

    enum State {
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
