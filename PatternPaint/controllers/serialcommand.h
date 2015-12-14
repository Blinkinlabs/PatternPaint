#ifndef SERIALCOMMAND_H
#define SERIALCOMMAND_H

#include <QString>
#include <QByteArray>

class SerialCommand
{
public:
    SerialCommand(QString name_, QByteArray command_, QByteArray expectedResponse_);

    SerialCommand(QString name_, QByteArray command_, QByteArray expectedResponse_,
                  QByteArray expectedResponseMask_);

    QString name;                   ///< Human-readable description of the command
    QByteArray commandData;         ///< Data to send to the bootloader
    QByteArray expectedResponse;    ///< Expected response from the bootloader
    QByteArray expectedResponseMask;///< Mask for interpreting expected response
};

#endif // SERIALCOMMAND_H
