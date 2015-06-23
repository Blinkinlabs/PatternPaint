#ifndef SERIALCOMMANDQUEUE_H
#define SERIALCOMMANDQUEUE_H

#include <QObject>
#include <QtSerialPort>
#include <QTimer>

// A command/response method to handle serial ports
// This is to allow nonblocking, asyncronous commands to
// be run against a serial port. Each command is expected
// to have a response with a known length and value. A
// command timeout handles devices that have become
// unresponsive.
class SerialCommandQueue : public QObject
{
    Q_OBJECT
public:
    explicit SerialCommandQueue(QObject *parent = 0);

    bool open(QSerialPortInfo info);
    void close();

    bool isConnected();

    // Queue a new command
    void queueCommand(QString name,
                      QByteArray data,
                      QByteArray expectedRespone);

    void queueCommand(QString name,
                      QByteArray data,
                      QByteArray expectedRespone,
                      QByteArray expectedResponseMask);

signals:
    void error(QString error);
    void commandFinished(QString command, QByteArray returnData);

public slots:
    // Handle receiving data from the serial port
    void handleReadData();

    // Handle an error from the serial port
    void handleSerialError(QSerialPort::SerialPortError serialError);

    // Self-enforced communications timeout
    void handleCommandTimeout();

private:
    struct Command {
        Command(QString name_,
                QByteArray command_,
                QByteArray expectedResponse_) :
            name(name_),
            commandData(command_),
            expectedResponse(expectedResponse_) {}

        Command(QString name_,
                QByteArray command_,
                QByteArray expectedResponse_,
                QByteArray expectedResponseMask_) :
            name(name_),
            commandData(command_),
            expectedResponse(expectedResponse_),
            expectedResponseMask(expectedResponseMask_) {}

        QString name;                   ///< Human-readable description of the command
        QByteArray commandData;         ///< Data to send to the bootloader
        QByteArray expectedResponse;    ///< Expected response from the bootloader
        QByteArray expectedResponseMask;///< Mask for interpreting expected response
    };

    QPointer<QSerialPort> serial;   ///< Serial device the programmer is attached to

    QQueue<Command> commandQueue;   ///< Queue of commands to send
    QByteArray responseData;        ///< Data received by the current command

    // Timer fires if a command has failed to complete quickly enough
    QPointer<QTimer> commandTimeoutTimer;

    // If there is another command in the queue, start processing it.
    void processCommandQueue();

    void resetState();
};

#endif // SERIALCOMMANDQUEUE_H
