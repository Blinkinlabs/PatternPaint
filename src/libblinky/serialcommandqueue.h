#ifndef SERIALCOMMANDQUEUE_H
#define SERIALCOMMANDQUEUE_H

#include "libblinkyglobal.h"

#include "serialcommand.h"

#include <QObject>
#include <QtSerialPort>
#include <QTimer>

// A command/response method to handle serial ports
// This is to allow nonblocking, asyncronous commands to
// be run against a serial port. Each command is expected
// to have a response with a known length and value. A
// command timeout handles devices that have become
// unresponsive.
class LIBBLINKY_EXPORT SerialCommandQueue : public QObject
{
    Q_OBJECT
public:
    SerialCommandQueue(QObject *parent = 0);

    bool open(QSerialPortInfo info);
    void close();

    bool isOpen();

    // Queue a new command
    void enqueue(const SerialCommand &command);
    void enqueue(const QList<SerialCommand> &commands);

    void flushQueue();

    int length();

signals:
    void errorOccured(QString error);
    void commandFinished(QString command, QByteArray returnData);
    void lastCommandFinished();

public slots:
    // Handle receiving data from the serial port
    void handleReadData();

    // Handle an error from the serial port
    void handleSerialError(QSerialPort::SerialPortError error);

    // Self-enforced communications timeout
    void handleCommandTimeout();

private:
    QPointer<QSerialPort> serial;   ///< Serial device the programmer is attached to

    QQueue<SerialCommand> queue;   ///< Queue of commands to send
    QByteArray responseData;        ///< Data received by the current command

    // Timer fires if a command has failed to complete quickly enough
    QTimer commandTimeoutTimer;

    // If there is another command in the queue, start processing it.
    void processQueue();
};

#endif // SERIALCOMMANDQUEUE_H
