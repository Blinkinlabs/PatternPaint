#ifndef SERIALCOMMAND_H
#define SERIALCOMMAND_H

#include <QString>
#include <QByteArray>

#define DEFAULT_COMMAND_TIMEOUT 200 // Default command timeout, in ms

class SerialCommand
{
public:
    SerialCommand(QString name, QByteArray command, QByteArray expectedResponse,
                  int timeout = DEFAULT_COMMAND_TIMEOUT);

    SerialCommand(QString name, QByteArray command, QByteArray expectedResponse,
                  QByteArray expectedResponseMask, int timeout = DEFAULT_COMMAND_TIMEOUT);

    QString name;                   ///< Human-readable description of the command
    QByteArray data;         ///< Data to send to the bootloader
    QByteArray expectedResponse;    ///< Expected response from the bootloader
    QByteArray expectedResponseMask;///< Mask for interpreting expected response
    int timeout;
};

#endif // SERIALCOMMAND_H
