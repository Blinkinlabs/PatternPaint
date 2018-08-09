#ifndef LEOBLINKYUPLOADER_H
#define LEOBLINKYUPLOADER_H

#include <QObject>

#include "blinkyuploader.h"
#include "serialcommandqueue.h"

class LeoBlinkyUploader : public BlinkyUploader
{
    Q_OBJECT

public:
    LeoBlinkyUploader(QObject *parent = 0);

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
    float progress;

    QString errorString;

    SerialCommandQueue commandQueue;
};

#endif // LEOBLINKYUPLOADER_H
