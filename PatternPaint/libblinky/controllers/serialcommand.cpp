#include "serialcommand.h"

SerialCommand::SerialCommand(QString name, QByteArray command, QByteArray expectedResponse,
                             int timeout) :
    name(name),
    data(command),
    expectedResponse(expectedResponse),
    timeout(timeout)
{
}

SerialCommand::SerialCommand(QString name, QByteArray command, QByteArray expectedResponse,
                             QByteArray expectedResponseMask, int timeout) :
    name(name),
    data(command),
    expectedResponse(expectedResponse),
    expectedResponseMask(expectedResponseMask),
    timeout(timeout)
{
}
