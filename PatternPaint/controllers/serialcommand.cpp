#include "serialcommand.h"

SerialCommand::SerialCommand(QString name_, QByteArray command_, QByteArray expectedResponse_) :
    name(name_),
    data(command_),
    expectedResponse(expectedResponse_)
{
}

SerialCommand::SerialCommand(QString name_, QByteArray command_, QByteArray expectedResponse_,
                             QByteArray expectedResponseMask_) :
    name(name_),
    data(command_),
    expectedResponse(expectedResponse_),
    expectedResponseMask(expectedResponseMask_)
{
}
