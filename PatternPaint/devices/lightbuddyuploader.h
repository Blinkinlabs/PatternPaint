#ifndef LIGHTBUDDYUPLOADER_H
#define LIGHTBUDDYUPLOADER_H

#include <QObject>
#include "patternuploader.h"
#include "lightbuddyprogrammer.h"


class LightBuddyUploader : public PatternUploader
{
    Q_OBJECT

public:
    LightBuddyUploader(QObject *parent=0);

    bool startUpload(BlinkyController& controller, std::vector<Pattern> patterns);
    bool upgradeFirmware(BlinkyController& controller);
    QString getErrorString() const;

private slots:
    void handleProgrammerError(QString error);

    void handleProgrammerCommandFinished(QString command, QByteArray returnData);

private:
    QString errorString;

    LightBuddyProgrammer programmer;
};

#endif // LIGHTBUDDYPLOADER_H
