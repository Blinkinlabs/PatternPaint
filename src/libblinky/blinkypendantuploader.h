#ifndef BLINKYPENDANTPATTERNUPLOADER_H
#define BLINKYPENDANTPATTERNUPLOADER_H

#include "libblinkyglobal.h"

#include <QObject>

#include "blinkyuploader.h"
#include "serialcommandqueue.h"

class LIBBLINKY_EXPORT BlinkyPendantUploader : public BlinkyUploader
{
    Q_OBJECT

public:
    BlinkyPendantUploader(QObject *parent = 0);

    bool storePatterns(BlinkyController &controller, QList<PatternWriter> &patternWriters);
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
    int progress;           ///< Current upload progress, in command counts
    int maxProgress;        ///< Total expected progress states

    QString errorString;

    SerialCommandQueue commandQueue;
};

#endif // BLINKYPENDANTPATTERNUPLOADER_H
