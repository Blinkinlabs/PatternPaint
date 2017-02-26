#include "firmwarestore.h"

#include <QFileDialog>
#include <QString>
#include <QDebug>
#include <QStandardPaths>

QStringList FirmwareStore::listAvailableFirmware() {
    QStringList firmwareNames;

    firmwareNames.push_back(DEFAULT_FIRMWARE_NAME);

    // search for third party Firmware
    QString documents = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    documents.append(FIRMWARE_FOLDER);
    QDir firmwareDir(documents);
    if (firmwareDir.exists()){
        QStringList firmwarelist = firmwareDir.entryList(QDir::Dirs);
        firmwarelist.removeFirst(); // TODO: Is this robust?
        firmwarelist.removeFirst();
        firmwareNames +=firmwarelist;
    }

    return firmwareNames;
}

QString FirmwareStore::getFirmwareDescription(const QString &name) {

    if(name==DEFAULT_FIRMWARE_NAME){
        return QString()
            .append("Default BlinkyTape Firmware\n")
            .append("Use this for all standard functions");
    }

    // read README.md
    QString documents = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    documents.append(FIRMWARE_FOLDER);
    documents.append(name);
    documents.append("/");
    documents.append(FIRMWARE_DESCRIPTION_FILE);

    QFile inputFile(documents);
    if (!inputFile.open(QIODevice::ReadOnly))
    {
        return QString()
            .append("No description available");
    }

    QString description;

    QTextStream in(&inputFile);
    description.append(in.readAll());

    inputFile.close();

    return description;
}

bool FirmwareStore::addFirmware(const QString &sourcePathName) {
    qDebug() << "Adding firmware from directory:" << sourcePathName;

    QDir sourceDirectory(sourcePathName);

    if (!sourceDirectory.exists()){
        errorString = "Source directory doesn't exist";
        return false;
    }

    QString sourceFirmwareName = sourcePathName;
    sourceFirmwareName.append("/");
    sourceFirmwareName.append(sourceDirectory.dirName());
    sourceFirmwareName.append(".hex");

    // Check if the hex file exists and is readable
    // TODO: Use the firmware loader to validate it as well
    if (!QFileInfo(sourceFirmwareName).isReadable())
    {
        errorString = "Firmware hex file not found";
        return false;
    }

    QString destinationPathName = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    destinationPathName.append(FIRMWARE_FOLDER);
    destinationPathName.append(sourceDirectory.dirName());

    QDir destinationDirectory(destinationPathName);

    if (destinationDirectory.exists()){
        errorString = "Firmware folder already exists, please remove first";
        return false;
    }

    if(!destinationDirectory.mkpath(".")){
        errorString = "Can't create firmware folder";
        return false;
    }

    QString destinationFirmwareName = destinationPathName;
    destinationFirmwareName.append("/");
    destinationFirmwareName.append(sourceDirectory.dirName());
    destinationFirmwareName.append(".hex");

    if(!QFile::copy(sourceFirmwareName, destinationFirmwareName)){
        errorString = "can not copy Firmware hex file";

        // TODO: This only needs to remove the directory...
        removeFirmware(sourceDirectory.dirName());
        return false;
    }

    // copy description file
    QString sourceReadmeName = sourcePathName;
    sourceReadmeName.append("/");
    sourceReadmeName.append(FIRMWARE_DESCRIPTION_FILE);

    QString destinationReadmeName = destinationPathName;
    destinationReadmeName.append("/");
    destinationReadmeName.append(FIRMWARE_DESCRIPTION_FILE);

    if(!QFile::copy(sourceReadmeName, destinationReadmeName)){
        qDebug() << "can not copy Firmware description file";
    }

    return true;
}

bool FirmwareStore::removeFirmware(const QString &name) {
    qDebug() << "remove Firmware " << name;

    if (name == DEFAULT_FIRMWARE_NAME) {
        errorString = "Cannot remove default firmware";
        return false;
    }

    QString documents = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    documents.append(FIRMWARE_FOLDER);
    documents.append(name);

    QDir firmwareDir(documents);

    if(!firmwareDir.removeRecursively()) {
        errorString = "Error removing firmware directory";
        return false;
    }

    return true;
}

QString FirmwareStore::getErrorString() const {
    return errorString;
}
