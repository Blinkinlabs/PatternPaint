#ifndef AVRPROGRAMMER_H
#define AVRPROGRAMMER_H

#include <QObject>
#include <QtSerialPort>
#include <QTimer>

// Given a serial object that is suspected to have an AVR109-compatible booloader
// attached to it (ie, caterina), use it to load a new user program.
//
// The commands are all non-blocking; they will proceed
class AvrProgrammer : public QObject
{
    Q_OBJECT
public:
    explicit AvrProgrammer(QObject *parent = 0);

    bool openSerial(QSerialPortInfo info);
    void closeSerial();

    bool isConnected();

    /// High-level interface functions

    /// Read the contents of the flash
    /// @param startAddress Word-aligned address to begin reading from, in bytes
    /// @param lengthBytes Length of data to read, in bytes
    void readFlash(int startAddress, int lengthBytes);

    /// Write the contents of the flash
    /// Note that if length is not a multiple of the page size, some pre-existing
    /// data at the end may be erased.
    /// @param data QByteArray containing the data to write to the flash
    /// @param startAddress Page-aligned address to begin writing to, in bytes
    void writeFlash(QByteArray& data, int startAddress);

    /// These functions are direct implementations of the bootloader interface

    // Set the current read/write address for flash/EEPROM reading/writing
    // Note: The address needs to be divisble by 2, because it is interpreted
    // as a word address by the bootloader.
    void setAddress(int address);

    // Check that we are talking to the correct device
    void checkDeviceSignature();

    // Instruct the programmer to reset. This has the side effect of deleting the device.
    void reset();

signals:
    void error(QString error);
    void commandFinished(QString command, QByteArray returnData);

private slots:
    // Handle receiving data from the serial port
    void handleReadData();

    // Handle an error from the serial port
    void handleSerialError(QSerialPort::SerialPortError error);

    // Self-enforced communications timeout
    void handleCommandTimeout();

private:
    struct Command {
        Command(QString name,
                QByteArray command,
                QByteArray expectedResponse) :
            commandName(name),
            commandData(command),
            expectedResponseData(expectedResponse) {}

        QString commandName;
        QByteArray commandData;
        QByteArray expectedResponseData;
    };

    QSerialPort* serial;  // Serial device the programmer is attached to

    QQueue<Command> commandQueue;
    QByteArray responseData;    // Data we actually received

    QTimer *commandTimeoutTimer; // Timer fires if a command has failed to complete quickly enough

    // Queue a new command
    void queueCommand(QString commandName, QByteArray commandData, QByteArray expectedResponseData);

    // If there is another command in the queue, start processing it.
    void processCommandQueue();
};

#endif // AVRPROGRAMMER_H
