#include "firmwareimport.h"

#include <QFileDialog>
#include <QString>
#include <QDebug>


QByteArray FIRMWARE_DATA;
QString FIRMWARE_NAME = DEFAULT_FIRMWARE_NAME;


bool firmwareimport::firmwareRead(const QString filename)
{
    // read HEX file
    qDebug() << "read firmware: " << filename;

    //read line
    QFile inputFile(filename);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       FIRMWARE_DATA.clear();
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
                          FIRMWARE_DATA.append(static_cast<char>(dataByte));
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
       qDebug() << "Firmware size: " << FIRMWARE_DATA.count() << "bytes";
       //qDebug() << FIRMWARE_DATA;
       return true;

    }

    return false;

}
