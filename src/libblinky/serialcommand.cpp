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

CompareResult SerialCommand::testResponse(const QByteArray &response)
{
    if(expectedResponse.length() > response.length()) {
        return RESPONSE_NOT_ENOUGH_DATA;
    }

    if(expectedResponse.length() < response.length()) {
        return RESPONSE_TOO_MUCH_DATA;
    }

    if(!expectedResponseMask.isNull()) {
        if(expectedResponseMask.length() != expectedResponse.length()) {
            return RESPONSE_INVALID_MASK;
        }

        for (int i = 0; i < response.length(); i++) {
            if (expectedResponseMask.at(i) != 0
                && response.at(i) != expectedResponse.at(i)) {
                qCritical() << "Got unexpected data back"
                            << "name:" << name
                            << "position:" << i
                            << "expected:" << (int)expectedResponse.at(i)
                            << "received:" << (int)response.at(i)
                            << "mask:" << (int)expectedResponseMask.at(i);

                return RESPONSE_MISMATCH;
            }
        }

        return RESPONSE_MATCH;
    }

    // Otherwise, just compare them directly
    if(expectedResponse == response) {
        return RESPONSE_MATCH;
    }
    else {
        return RESPONSE_MISMATCH;
    }
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
