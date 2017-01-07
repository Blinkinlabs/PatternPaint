#include "serialcommand.h"

SerialCommand::SerialCommand(QString name, QByteArray data, QByteArray expectedResponse,
                             int timeout) :
    name(name),
    data(data),
    expectedResponse(expectedResponse),
    timeout(timeout)
{
}

SerialCommand::SerialCommand(QString name, QByteArray data, QByteArray expectedResponse,
                             QByteArray expectedResponseMask, int timeout) :
    name(name),
    data(data),
    expectedResponse(expectedResponse),
    expectedResponseMask(expectedResponseMask),
    timeout(timeout)
{
}
