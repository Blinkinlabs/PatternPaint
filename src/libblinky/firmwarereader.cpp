#include "firmwarereader.h"

#include <QDebug>
#include <QFile>

bool parseHexLine(QString line,
                  unsigned int &address,
                  unsigned int &type,
                  QByteArray &data)
{
    bool result;

    // Check minimum length
    if(line.length() < 11) {
        qDebug() << "Line length too short";
        return false;
    }

    //read colon
    if(line.at(0)!=':'){
        qDebug() << "Format error in hex file, colon is missing";
        return false;
    }

    //read data length
    unsigned int dataLength = line.mid(1,2).toUInt(&result,16);
    if (!result){
        qDebug() << "Format error in hex file, data length can not be read";
        return false;
    }

    // Test that the line length is correct
    if (int(11+dataLength*2)!=line.length()){
        qDebug() << "Format error in hex file, incorrect length";
        return false;
    }

    //read data address
    address = line.mid(3,4).toUInt(&result,16);
    if(!result){
        qDebug() << "Format error in hex file, address can not be read";
        return false;
    }

    //read data type
    type = line.mid(7,2).toUInt(&result,16);
    if(!result){
        qDebug() << "Format error in hex file, data type can not be read";
        return false;
    }

    //read checksum
    line.mid(line.length()-2,2).toUInt(&result,16);
    if(!result){
        qDebug("Format error in hex file, checksum can not be read");
        return false;
    }

    // calculate checksum
    unsigned int checksumData = 0;
    for(int i=1;i<line.length(); i+=2){
        checksumData += line.mid(i,2).toUInt(&result,16);
    }
    if(checksumData & 0xFF){
        qDebug("Format error in hex file, checksum is incorrect");
        return false;
    }

    // Read out the data
    data.clear();
    for(unsigned int i=0; i<(dataLength*2); i+=2){
        //read data
        uint8_t dataByte = line.mid(i+9,2).toUInt(&result,16);
        if(!result){
            qDebug() << "Format error in hex file, data can not be read";
            return false;
        }
        data.append(static_cast<char>(dataByte));
    }

    return true;
}

const QByteArray& FirmwareReader::getData() const {
    return data;
}

bool FirmwareReader::load(const QString& fileName)
{
    qDebug() << "read firmware: " << fileName;

    QFile inputFile(fileName);
    if (!inputFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Cannot open hex file for reading";
        return false;
    }

    data.clear();
    unsigned int nextAddress = 0;

    QTextStream in(&inputFile);
    while (!in.atEnd()){
        unsigned int address;
        unsigned int type;
        QByteArray lineData;

        //read line
        if(!parseHexLine(in.readLine(), address, type, lineData)) {
            return false;
        }

        switch(type){

        // data
        case 0x00:
            if(nextAddress != address){
                qDebug("Format error in hex file, address not contiguous");
                return false;
            }

            data.append(lineData);
            nextAddress += lineData.count();

            break;

        // end of file
        case 0x01:
            break;

        default:
            qDebug() << "Format error in hex file, data type not recognized";
            return false;
        }
    }

    inputFile.close();
    qDebug() << "Firmware size:" << data.count();

    return true;
}
