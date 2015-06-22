#ifndef LIGHTBUDDYPROTOCOL_H
#define LIGHTBUDDYPROTOCOL_H

#include <QObject>
#include <QtSerialPort>
#include <QTimer>


/// Interact with the light buddy controller over serial
class LightBuddyProtocol : public QObject
{
    Q_OBJECT
public:
    explicit LightBuddyProtocol(QObject *parent = 0);

//    bool commandProgramAddress(uint8_t* buffer);
//    bool commandReloadAnimations(uint8_t* buffer);
//    bool commandFreeSpace(uint8_t* buffer);

    /// Get the size of the largest file that can be stored
    void getLargestfile();

//    bool commandFileNew(uint8_t* buffer);
//    bool commandFileWritePage(uint8_t* buffer);
//    bool commandFileRead(uint8_t* buffer);
//    bool commandFileCount(uint8_t* buffer);
//    bool commandFirstFreeSector(uint8_t* buffer);
//    bool commandFileGetType(uint8_t* buffer);
//    bool commandFileDelete(uint8_t* buffer);
//    bool commandFlashErase(uint8_t* buffer);
//    bool commandFlashRead(uint8_t* buffer);

    bool open(QSerialPortInfo info);
    void close();

    bool isConnected();

    // Queue a new command
    void queueCommand(QString name, QByteArray data, QByteArray expectedRespone);

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

        QString name;                   ///< Human-readable description of the command
        QByteArray commandData;         ///< Data to send to the bootloader
        QByteArray expectedResponse;    ///< Data we expect back from the bootloader
    };

    QPointer<QSerialPort> serial;   ///< Serial device the programmer is attached to

    QQueue<Command> commandQueue;   ///< Queue of commands to send
    QByteArray responseData;        ///< Data received by the current command

    // Timer fires if a command has failed to complete quickly enough
    QPointer<QTimer> commandTimeoutTimer;

    // If there is another command in the queue, start processing it.
    void processCommandQueue();
};

#endif // LIGHTBUDDYPROTOCOL_H
