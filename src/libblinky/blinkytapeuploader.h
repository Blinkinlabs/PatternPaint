#ifndef BLINKYTAPEUPLOADER_H
#define BLINKYTAPEUPLOADER_H

#include "libblinkyglobal.h"

#include "patternwriter.h"
#include "blinkyuploader.h"
#include "memorysection.h"
#include "avr109firmwareloader.h"

#include <QObject>
#include <QTimer>
#include <iostream>

// BlinkyTape pattern uploader. Assembles a firmware image that contains a player sketch, pattern table,
// and pattern data, then uploads it using the Avr109 protocol.
class LIBBLINKY_EXPORT BlinkyTapeUploader : public BlinkyUploader
{
    Q_OBJECT

public:
    BlinkyTapeUploader(QObject *parent = 0);

    bool storePatterns(BlinkyController &blinky, QList<PatternWriter> &patternWriters);
    QString getErrorString() const;

    QList<PatternWriter::Encoding> getSupportedEncodings() const;

public slots:
    void cancel();

private:
    Avr109FirmwareLoader firmwareLoader;

    QString errorString;
};

#endif // BLINKYTAPEUPLOADER_H
