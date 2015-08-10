#include "lightbuddyuploader.h"

LightBuddyUploader::LightBuddyUploader(QObject *parent) :
    PatternUploader(parent)
{
    connect(&programmer,SIGNAL(error(QString)),
            this,SLOT(handleProgrammerError(QString)));
    connect(&programmer,SIGNAL(commandFinished(QString,QByteArray)),
            this,SLOT(handleProgrammerCommandFinished(QString,QByteArray)));
}


bool LightBuddyUploader::startUpload(BlinkyController& controller, std::vector<Pattern> patterns)
{
    Q_UNUSED(controller);
    Q_UNUSED(patterns);

    // Upload procedure:
    // 1. Erase the flash (slow! TODO: new firmware that's faster)
    // 2. For each pattern:
    //    a. create a new file
    //    b. if creation successful, upload image data
    // 3. Reset controller

    errorString = "Pattern upload not currently supported for Lightbuddy!";
    return false;
}

bool LightBuddyUploader::upgradeFirmware(BlinkyController& controller)
{
    Q_UNUSED(controller);

    // TODO: Support firmware upload for the lightbuddy
    errorString = "Firmware update not currently supported for Lightbuddy!";
    return false;
}

bool LightBuddyUploader::upgradeFirmware(int)
{
    // TODO: Support firmware upload for the lightbuddy
    errorString = "Firmware update not currently supported for Lightbuddy!";
    return false;
}

QString LightBuddyUploader::getErrorString() const
{
    return errorString;
}

void LightBuddyUploader::handleProgrammerError(QString error) {
    qCritical() << error;

// TODO
//    if(programmer.isConnected()) {
//        programmer.close();
//    }

//    emit(finished(false));
}

void LightBuddyUploader::handleProgrammerCommandFinished(QString command, QByteArray returnData) {
    Q_UNUSED(command);
    Q_UNUSED(returnData);

// TODO
//    qDebug() << "Command finished:" << command;
//    setProgress(progress + 1);

//    // TODO: Let the receiver handle this instead.
//    if(command == "stopWrite") {
//        programmer.close();
//        emit(finished(true));
//    }
}
