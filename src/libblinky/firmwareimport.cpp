#include "firmwareimport.h"

#include <QFileDialog>
#include <QString>
#include <QDebug>
#include <QStandardPaths>

QStringList firmwareimport::listAvailableFirmware() {
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

QString firmwareimport::getFirmwareDescription(const QString &name) {

    if(name==DEFAULT_FIRMWARE_NAME){
        return QString()
            .append("Default BlinkyTape Firmware\n")
            .append("Use this for all standard functions");
    }

    // read README.md
    QString documents = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    documents.append(FIRMWARE_FOLDER);
    documents.append(name);
    documents.append("/README.md");

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

bool firmwareimport::removeFirmware(const QString &name) {

    if (name == DEFAULT_FIRMWARE_NAME) {
        return false;
    }

    qDebug() << "remove Firmware:" << name;

    QString documents = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    documents.append(FIRMWARE_FOLDER);
    documents.append(name);
    QDir firmwareDir(documents);

    if(!firmwareDir.exists()) {
        return false;
    }

    firmwareDir.removeRecursively();
    return true;
}

bool firmwareimport::addFirmware(const QString &dirSource) {

    QDir firmwareDirSource(dirSource);
    if (!firmwareDirSource.exists()){
        return false;
    }

    // TODO: Test if there is at least a valid .hex file in the directory before doing anything.

    // create firmwarefolder
    QString dirDestination = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    dirDestination.append(FIRMWARE_FOLDER);
    dirDestination.append(firmwareDirSource.dirName());
    QDir firmwareDirDestination(dirDestination);
    if (!firmwareDirDestination.exists()){
        qDebug() << "Firmware folder is created";
        firmwareDirDestination.mkpath(".");
    }else{
        qDebug() << "Firmware folder already exists";
    }

    // TODO: Bail if the destination directory is not created

    // copy hex file
    QString fileSource = dirSource;
    fileSource.append("/");
    fileSource.append(firmwareDirSource.dirName());
    fileSource.append(".hex");

    QString fileDestination = dirDestination;
    fileDestination.append("/");
    fileDestination.append(firmwareDirSource.dirName());
    fileDestination.append(".hex");

    QFile hexFile(fileSource);
    if (hexFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "copy hex file" ;
        QFile::copy(fileSource, fileDestination);
    }else{
        qDebug() << "hex file not found";
        return false;
    }

    // copy README.md file
    fileSource = dirSource;
    fileSource.append("/README.md");

    fileDestination = dirDestination;
    fileDestination.append("/README.md");

    QFile readmeFile(fileSource);
    if (readmeFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "copy README.md";
        QFile::copy(fileSource, fileDestination);
    }else{
        qDebug() << "README.md not found";
    }

    return true;
}


const QByteArray& firmwareimport::getData() const {
    return data;
}

const QString& firmwareimport::getName() const {
    return name;
}

bool firmwareimport::firmwareRead(const QString& filename)
{
    // read HEX file
    qDebug() << "read firmware: " << filename;

    //read line
    QFile inputFile(filename);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       data.clear();
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          //read colon
          QString line = in.readLine();
          if(line.at(0)==':'){
              //read data lenght
              bool bStatus = false;
              unsigned int dataLenght = line.mid(1,2).toUInt(&bStatus,16);
              if(bStatus==true){
                  for(unsigned int i=0; i<dataLenght<<1; i+=2){
                      //read data
                      uint8_t dataByte = line.mid(i+9,2).toUInt(&bStatus,16);
                      if(bStatus==true){
                          data.append(static_cast<char>(dataByte));
                      }else{
                          qDebug("Format error in hex file, data can not be read");
                          return false;
                      }
                  }
              }else{
                  qDebug("Format error in hex file, address can not be read");
                  return false;
              }
          }else{
              qDebug("Format error in hex file, colon is missing");
              return false;
          }
       }
       inputFile.close();
       qDebug() << "Firmware size: " << data.count() << "bytes";
       //qDebug() << FIRMWARE_DATA;
       return true;

    }

    return false;

}
