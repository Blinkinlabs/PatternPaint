#include "firmwarestore.h"
#include "firmwarereader.h"

#include <QFileDialog>
#include <QString>
#include <QDebug>
#include <QStandardPaths>


QStringList FirmwareStore::listFirmwareSearchPaths()
{
    QStringList firmwarePaths;

    // Built-in firmware for BlinkyTape
    firmwarePaths.push_back(":/firmware/blinkytape/");

    // Search path for third party firmware
    firmwarePaths.push_back(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).append(FIRMWARE_FOLDER));

    return firmwarePaths;
}

QStringList FirmwareStore::listAvailableFirmware()
{
    QStringList firmwareNames;

    for(QString firmwarePath : listFirmwareSearchPaths()) {
        QDir directory(firmwarePath);
        if (!directory.exists())
            continue;

        firmwareNames.append(directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot));
    }

    return firmwareNames;
}

QString FirmwareStore::getFirmwareDirectoryName(const QString &name)
{
    for(QString firmwarePath : listFirmwareSearchPaths()) {
        QDir directory(firmwarePath);
        if (directory.exists(name))
            return directory.absoluteFilePath(name);
    }

    return QString();
}


QString FirmwareStore::getFirmwareDescription(const QString &name)
{
    QString directoryName = getFirmwareDirectoryName(name);

    if(directoryName.isNull())
        return QString();

    // read README.md
    QString filename;
    filename.append(directoryName);
    filename.append("/");
    filename.append(FIRMWARE_DESCRIPTION_FILE);

    QFile readmeFile(filename);
    if (!readmeFile.open(QIODevice::ReadOnly))
        return QString().append("No description available");

    QString description;

    QTextStream in(&readmeFile);
    description.append(in.readAll());

    readmeFile.close();

    return description;
}

QByteArray FirmwareStore::getFirmwareData(const QString &name)
{
    QString directoryName = getFirmwareDirectoryName(name);

    if(directoryName.isNull())
        return QByteArray();

    QString filename;
    filename.append(directoryName);
    filename.append("/");
    filename.append(name);
    filename.append(".hex");

    FirmwareReader reader;
    if(!reader.load(filename)) {
        return QByteArray();
    }

    return reader.getData();
}


bool FirmwareStore::addFirmware(const QString &sourcePathName)
{
    QDir sourceDirectory(sourcePathName);

    if(listAvailableFirmware().contains(sourceDirectory.dirName())) {
        errorString = "Firmware with this name already exists, please use a different name!";
        return false;
    }

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

    if(!QFile::copy(sourceReadmeName, destinationReadmeName))
        qDebug() << "can not copy Firmware description file";

    return true;
}

bool FirmwareStore::removeFirmware(const QString &name)
{

    QString firmwareDirectoryName = getFirmwareDirectoryName(name);

    if(firmwareDirectoryName.isNull()) {
        errorString = "No firmware with that name found";
        return false;
    }

    if(firmwareDirectoryName.startsWith(":/")) {
        errorString = "Cannot remove built-in firmware";
        return false;
    }

    QDir firmwareDirectory(firmwareDirectoryName);
    if(!firmwareDirectory.removeRecursively()) {
        errorString = "Error removing firmware directory, please check manually";
        return false;
    }

    return true;
}

QString FirmwareStore::getErrorString() const
{
    return errorString;
}
