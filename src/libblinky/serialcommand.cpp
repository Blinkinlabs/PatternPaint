#include "serialcommand.h"

#include <QDebug>

SerialCommand::SerialCommand(QString name, const QByteArray &data, const QByteArray &expectedResponse,
                             int timeout) :
    name(name),
    data(data),
    expectedResponse(expectedResponse),
    timeout(timeout)
{
}

SerialCommand::SerialCommand(QString name, const QByteArray &data, const QByteArray &expectedResponse,
                             const QByteArray &expectedResponseMask, int timeout) :
    SerialCommand(name, data, expectedResponse, timeout)
{
    // TODO: should we give feedback here if the expectedResponse is not equal to expectedResponseMask?
    this->expectedResponseMask = expectedResponseMask;
}

SerialCommand::CompareResult SerialCommand::testResponse(const QByteArray &response)
{
    if(expectedResponse.length() > response.length()) {
        return RESPONSE_NOT_ENOUGH_DATA;
    }

    if(expectedResponse.length() < response.length()) {
        errorString = QString()
                .append("Got more data than we expected")
                .append(" command name: %1").arg(name)
                .append(" expected: %1 ").arg(expectedResponse.length())
                .append(" received: %1").arg(response.length());
        return RESPONSE_TOO_MUCH_DATA;
    }

    if(!expectedResponseMask.isNull()) {
        if(expectedResponseMask.length() != expectedResponse.length()) {
            errorString = QString()
                    .append("Invalid mask length: command formatted incorrectly.")
                    .append(" command name: %1").arg(name)
                    .append(" expectedResponse: %1").arg(expectedResponse.length())
                    .append(" expectedResponseMask: %1").arg(expectedResponseMask.length());
            return RESPONSE_INVALID_MASK;
        }

        for (int i = 0; i < response.length(); i++) {
            if (expectedResponseMask.at(i) != 0
                && response.at(i) != expectedResponse.at(i)) {
                errorString = QString()
                        .append("Got unexpected data back")
                        .append(" command name: %1").arg(name)
                        .append(" position: %1").arg(i)
                        .append(" expected: %1").arg(expectedResponse.at(i),2,16)
                        .append(" received: %1").arg(response.at(i),2,16)
                        .append(" mask: %1").arg(expectedResponseMask.at(i),2,16);

                return RESPONSE_MISMATCH;
            }
        }

        return RESPONSE_MATCH;
    }

    for (int i = 0; i < response.length(); i++) {
        if (response.at(i) != expectedResponse.at(i)) {
            errorString = QString()
                    .append("Got unexpected data back")
                    .append(" name:").append(name)
                    .append(" position: %1").arg(i)
                    .append(" expected: %1").arg(expectedResponse.at(i),2,16)
                    .append(" received: %1").arg(response.at(i),2,16);

            return RESPONSE_MISMATCH;
        }
    }

    return RESPONSE_MATCH;
}

bool SerialCommand::operator==(const SerialCommand& rhs) const
{
    return ((name == rhs.name)
            && (data == rhs.data)
            && (expectedResponse == rhs.expectedResponse)
            && (expectedResponseMask == rhs.expectedResponseMask)
            && (timeout == rhs.timeout)
            );
}

QString SerialCommand::getErrorString() const
{
    return errorString;
}
