#ifndef LIGHTBUDDYUPLOADER_H
#define LIGHTBUDDYUPLOADER_H

#include <QObject>
#include "blinkyuploader.h"
#include "lightbuddyprogrammer.h"


class LightBuddyUploader : public BlinkyUploader
{
    Q_OBJECT

public:
    LightBuddyUploader(QObject *parent=0);

    bool startUpload(BlinkyController& controller, std::vector<PatternWriter> patterns);
    bool upgradeFirmware(BlinkyController& controller);
    bool upgradeFirmware(int timeout);
    QString getErrorString() const;

    QList<PatternWriter::Encoding> getSupportedEncodings() const;

public slots:
    void cancel();

private slots:
    void doWork();  /// Handle the next section of work, whatever it is

    void handleProgrammerError(QString error);

    void handleProgrammerCommandFinished(QString command, QByteArray returnData);

private:
    QString errorString;

    LightBuddyProgrammer programmer;

    /// Update any listeners with the maximum progress
    void setMaxProgress(int newProgress);

    /// Update any listeners with the latest progress
    void setProgress(int newProgress);

    enum State {
        State_EraseFlash,                    ///< Erase the flash
        State_FileNew,                         ///< Time to create a new file
        State_WriteFileData,                   ///< Write the data for the latest file
        State_Done                 ///< Next step is to
    };

    State state;

    QQueue<QByteArray> flashData; ///< Queue of memory sections to write
};

#endif // LIGHTBUDDYPLOADER_H
