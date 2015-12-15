#ifndef LIGHTBUDDYUPLOADER_H
#define LIGHTBUDDYUPLOADER_H

#include "blinkyuploader.h"
#include "serialcommandqueue.h"

#include <QObject>

class LightBuddyUploader : public BlinkyUploader
{
    Q_OBJECT

public:
    LightBuddyUploader(QObject *parent = 0);

    bool startUpload(BlinkyController &controller, std::vector<PatternWriter> patternWriters);
    bool upgradeFirmware(BlinkyController &controller);
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
    QString errorString;

    SerialCommandQueue commandQueue;

    /// Update any listeners with the latest progress
    void setProgress(int newProgress);

    enum State {
        State_EraseFlash,                    ///< Erase the flash
        State_FileNew,                         ///< Time to create a new file
        State_WriteFileData,                   ///< Write the data for the latest file
        State_Done                 ///< Next step is to
    };

    State state;

    int progress;
    int maxProgress;

    int sector; /// Current file sector that is being written to/read from

    QQueue<QByteArray> flashData; ///< Queue of memory sections to write
};

#endif // LIGHTBUDDYPLOADER_H
