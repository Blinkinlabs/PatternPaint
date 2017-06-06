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

    bool isOpen() const;

    // Queue a new command
    void enqueue(const SerialCommand &command);
    void enqueue(const QList<SerialCommand> &commands);

    void flushQueue();

    int length() const;

signals:
    // Called periodically during long-running commands.
    void commandStillRunning(QString command);

    // A specific command has finished. Signalled at the completion
    // of every command
    void commandFinished(QString command, QByteArray returnData);

    // All commands in the queue have finished
    void lastCommandFinished();

    // An error has occured, and queue processing has stopped.
    void errorOccured(QString error);

private slots:
    // Handle receiving data from the serial port
    void handleReadData();

    // Handle an error from the serial port
    void handleSerialError(QSerialPort::SerialPortError error);

    // Self-enforced communications timeout
    void handleCommandTimerTimeout();

private:
    QPointer<QSerialPort> serial;   ///< Serial device the programmer is attached to

    QQueue<SerialCommand> queue;   ///< Queue of commands to send
    QByteArray responseData;        ///< Data received by the current command

    // Timer fires if a command has failed to complete quickly enough
    int commandTimeRemaining;
    QTimer commandTimeoutTimer;

    // If there is another command in the queue, start processing it.
    void processQueue();
};

#endif // SERIALCOMMANDQUEUE_H
