#ifndef BLINKYPENDANTPATTERNUPLOADER_H
#define BLINKYPENDANTPATTERNUPLOADER_H

#include <QObject>

#include "patternuploader.h"
#include "blinkypendantprogrammer.h"

class BlinkyPendantUploader : public PatternUploader
{
    Q_OBJECT

public:
    BlinkyPendantUploader(QObject *parent=0);

    bool startUpload(BlinkyController& controller, std::vector<PatternWriter> patterns);
    bool upgradeFirmware(BlinkyController& controller);
    bool upgradeFirmware(int timeout);
    QString getErrorString() const;

    QList<PatternWriter::Encoding> getSupportedEncodings() const;

private slots:
    void handleProgrammerError(QString error);

    void handleProgrammerCommandFinished(QString command, QByteArray returnData);

private:
    /// Update any listeners with the maximum progress
    void setMaxProgress(int newProgress);

    /// Update any listeners with the latest progress
    void setProgress(int newProgress);

    /// Current upload progress, in command counts
    float progress;

    QString errorString;

    BlinkyPendantProgrammer programmer;
};

#endif // BLINKYPENDANTPATTERNUPLOADER_H
