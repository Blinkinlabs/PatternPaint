#ifndef SERIALCOMMAND_H
#define SERIALCOMMAND_H

#include "libblinkyglobal.h"

#include <QMetaType>
#include <QString>
#include <QByteArray>

#define DEFAULT_COMMAND_TIMEOUT 5000 // Default command timeout, in ms

class LIBBLINKY_EXPORT SerialCommand
{
public:
    enum CompareResult {
        RESPONSE_INVALID_MASK,     // TODO: Don't allow this condition in the first place
        RESPONSE_MISMATCH,         // Response did not match expected response
        RESPONSE_NOT_ENOUGH_DATA,  // Not enough response data yet
        RESPONSE_TOO_MUCH_DATA,     // Got too much data
        RESPONSE_MATCH,             // Response matched expected response
    };

    SerialCommand(QString name, const QByteArray &data, const QByteArray &expectedResponse,
                  int timeout = DEFAULT_COMMAND_TIMEOUT);

    SerialCommand(QString name, const QByteArray &data, const QByteArray &expectedResponse,
                  const QByteArray &expectedResponseMask,
                  int timeout = DEFAULT_COMMAND_TIMEOUT);

    CompareResult testResponse(const QByteArray &response);

    QString name;                   ///< Human-readable description of the command
    QByteArray data;                ///< Data to send to the bootloader
    QByteArray expectedResponse;    ///< Expected response from the bootloader
    QByteArray expectedResponseMask;///< Mask for interpreting expected response (0 is ignore)
    int timeout;

    bool operator==(const SerialCommand& rhs) const;

    QString getErrorString() const;

private:
    QString errorString;
};

Q_DECLARE_METATYPE(SerialCommand::CompareResult)

#endif // SERIALCOMMAND_H
