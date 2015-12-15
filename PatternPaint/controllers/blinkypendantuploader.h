#ifndef BLINKYPENDANTPATTERNUPLOADER_H
#define BLINKYPENDANTPATTERNUPLOADER_H

#include <QObject>

#include "blinkyuploader.h"
#include "serialcommandqueue.h"

class BlinkyPendantUploader : public BlinkyUploader
{
    Q_OBJECT

public:
    BlinkyPendantUploader(QObject *parent = 0);

    bool startUpload(BlinkyController &controller, std::vector<PatternWriter> patterns);
    bool upgradeFirmware(BlinkyController &controller);
    bool upgradeFirmware(int timeout);
    QString getErrorString() const;

    QList<PatternWriter::Encoding> getSupportedEncodings() const;

public slots:
    void cancel();

private slots:
    void handleError(QString error);

    void handleCommandFinished(QString command, QByteArray returnData);

private:
    /// Update any listeners with the latest progress
    void setProgress(int newProgress);

    /// Current upload progress, in command counts
    float progress;

    QString errorString;

    SerialCommandQueue commandQueue;
};

#endif // BLINKYPENDANTPATTERNUPLOADER_H
