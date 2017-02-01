#include "firmwareimport.h"

#include <QFileDialog>
#include <QString>
#include <QDebug>
#include <QStandardPaths>


#define BUFF_LENGTH 100
char buff[BUFF_LENGTH];


QString errorStringFirmware;


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

bool firmwareimport::removeFirmware(const QString &name) {

    if (name == DEFAULT_FIRMWARE_NAME) {
        return false;
    }

    snprintf(buff, BUFF_LENGTH,"remove Firmware: %s",(const char *)((QByteArray)name.toLatin1()).data());
    qDebug() << buff;

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

    snprintf(buff, BUFF_LENGTH,"add firmware: %s",(const char *)((QByteArray)(firmwareDirSource.dirName().toLatin1()).data()));
    qDebug() << buff;

    // create firmwarefolder
    QString dirDestination = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    dirDestination.append(FIRMWARE_FOLDER);
    dirDestination.append(firmwareDirSource.dirName());
    QDir firmwareDirDestination(dirDestination);
    if (!firmwareDirDestination.exists()){
        if(firmwareDirDestination.mkpath(".")){
            qDebug() << "Firmware folder is created";
        }else{
            errorStringFirmware = "can not create firmware folder";
            return false;
        }
    }else{
        errorStringFirmware = "Firmware folder already exists, please remove first";
        return false;
    }

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
        if(QFile::copy(fileSource, fileDestination)){
            snprintf(buff, BUFF_LENGTH,"copy Firmware file: %s.hex",(const char *)((QByteArray)(firmwareDirSource.dirName().toLatin1()).data()));
            qDebug() << buff;
        }else{
            errorStringFirmware = "can not copy Firmware hex file";
            removeFirmware(firmwareDirSource.dirName());
            return false;
        }
    }else{
        errorStringFirmware = "Firmware hex file not found";
        removeFirmware(firmwareDirSource.dirName());
        return false;
    }

    // copy description file
    fileSource = dirSource;
    fileSource.append("/");
    fileSource.append(FIRMWARE_DESCRIPTION_FILE);

    fileDestination = dirDestination;
    fileDestination.append("/");
    fileDestination.append(FIRMWARE_DESCRIPTION_FILE);

    QFile readmeFile(fileSource);
    if (readmeFile.open(QIODevice::ReadOnly))
    {
        if(QFile::copy(fileSource, fileDestination)){
            snprintf(buff, BUFF_LENGTH,"copy Firmware description: %s",FIRMWARE_DESCRIPTION_FILE);
            qDebug() << buff;
        }else{
            qDebug() << "can not copy Firmware description file";
        }
    }else{
        qDebug() << "no Firmware description file found";
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

    QFile inputFile(filename);
    if (!inputFile.open(QIODevice::ReadOnly))
    {
        qDebug("hex file can not be read");
        return false;
    }

    data.clear();
    unsigned int dataAddressNext = 0;

    QTextStream in(&inputFile);
    while (!in.atEnd()){

        //read line
        QString line = in.readLine();
        bool bStatus;

        //read colon
        if(line.at(0)!=':'){
            qDebug("Format error in hex file, colon is missing");
            return false;
        }

        //read data lenght
        unsigned int dataLenght = line.mid(1,2).toUInt(&bStatus,16);
        if(!bStatus){
            qDebug("Format error in hex file, data lenght can not be read");
            return false;
        }

        //read data address
        unsigned int dataAddress = line.mid(3,4).toUInt(&bStatus,16);
        if(!bStatus){
            qDebug("Format error in hex file, address can not be read");
            return false;
        }

        //read data type
        unsigned int dataType = line.mid(7,2).toUInt(&bStatus,16);
        if(!bStatus){
            qDebug("Format error in hex file, data type can not be read");
            return false;
        }


        switch(dataType){

        // data
        case 0x00:
            if(int(11+dataLenght*2)!=line.length()){
                qDebug("Format error in hex file, incorrect data length");
                return false;
            }else if(dataAddressNext != dataAddress){
                qDebug("Format error in hex file, address is false");
                return false;
            }
            dataAddressNext = dataAddress + dataLenght;

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

            break;

        // end of file
        case 0x01:
            break;

        default:
            qDebug("Format error in hex file, data type not recognized");
            return false;

        }


        //read checksum
        unsigned int checksum = line.mid(line.length()-2,2).toUInt(&bStatus,16);
        if(!bStatus){
            qDebug("Format error in hex file, checksum can not be read");
            return false;
        }

        // calculate checksum
        unsigned int checksumData = 0;
        for(int i=1;i<line.length(); i+=2){
            checksumData += line.mid(i,2).toUInt(&bStatus,16);
        }
        if(checksumData & 0xFF){
            qDebug("Format error in hex file, checksum is false");
            return false;
        }

    }


    inputFile.close();
    snprintf(buff, BUFF_LENGTH,"Firmware size: %iB",data.count());
    qDebug() << buff;

    return true;

}
