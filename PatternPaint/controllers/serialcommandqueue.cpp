#include "serialcommandqueue.h"

#define COMMAND_TIMEOUT_TIME 20000

SerialCommandQueue::SerialCommandQueue(QObject *parent) : QObject(parent)
{
    serial = new QSerialPort(this);
    serial->setSettingsRestoredOnClose(false);

    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)),
            this, SLOT(handleSerialError(QSerialPort::SerialPortError)));
    connect(serial,SIGNAL(readyRead()),this,SLOT(handleReadData()));

    commandTimeoutTimer = new QTimer(this);
    commandTimeoutTimer->setSingleShot(true);

    connect(commandTimeoutTimer, SIGNAL(timeout()),
            this, SLOT(handleCommandTimeout()));
}


bool SerialCommandQueue::open(QSerialPortInfo info) {
    if(isConnected()) {
        qCritical("Already connected to serial device");
        return false;
    }

    qDebug() << "connecting to " << info.portName();

#if defined(Q_OS_OSX)
    // Note: This should be info.portName(). Changed here as a workaround for:
    // https://bugreports.qt.io/browse/QTBUG-45127
    serial->setPortName(info.systemLocation());
#else
    serial->setPortName(info.portName());
#endif
    serial->setBaudRate(QSerialPort::Baud115200);

    if( !serial->open(QIODevice::ReadWrite) ) {
        qDebug() << "Could not connect to serial device. Error: " << serial->error() << serial->errorString();
        return false;
    }

    return true;
}

void SerialCommandQueue::close() {
    if(serial->isOpen()) {
        serial->close();
    }
}

bool SerialCommandQueue::isConnected() {
    return serial->isOpen();
}

void SerialCommandQueue::queueCommand(QString name,
                                 QByteArray data,
                                 QByteArray expectedRespone) {

//    qDebug() << "Queueing command:" << name;

    commandQueue.push_back(Command(name, data, expectedRespone));

    // Try to start processing commands.
    processCommandQueue();
}

void SerialCommandQueue::queueCommand(QString name,
                                 QByteArray data,
                                 QByteArray expectedRespone,
                                 QByteArray expectedResponseMask) {

//    qDebug() << "Queueing command:" << name;

    commandQueue.push_back(Command(name, data, expectedRespone, expectedResponseMask));

    // Try to start processing commands.
    processCommandQueue();
}

void SerialCommandQueue::processCommandQueue() {
//    for(int i = 0; i < commandQueue.count(); i++)
//        qDebug() << i << ": " << commandQueue.at(i).name;

    // Nothing to do if we don't have any commands...
    if(commandQueue.count() == 0) {
        return;
    }

    // Don't start a new command if one is already in progress
    if(commandTimeoutTimer->isActive()) {
        return;
    }

    if(!isConnected()) {
        qCritical() << "Device disappeared, cannot run command";
        return;
    }

    qDebug() << "Starting Command:" << commandQueue.front().name;
    responseData.clear();


    if(serial->write(commandQueue.front().commandData) != commandQueue.front().commandData.length()) {
        qCritical() << "Error writing to device";
        return;
    }

//    for(int i = 0; i < commandQueue.front().commandData.count(); i++)
//        qDebug() << "Data at " << i << ": "
//                 << (int)commandQueue.front().commandData.at(i)
//                 << "(" << commandQueue.front().commandData.at(i) << ")";

    // Start the timer; the command must complete before it fires, or it
    // is considered an error. This is to prevent a misbehaving device from hanging
    // the programmer code.
    commandTimeoutTimer->start(COMMAND_TIMEOUT_TIME);
}

void SerialCommandQueue::handleReadData() {
    if(commandQueue.length() == 0) {
        // TODO: error, we got unexpected data.
        qCritical() << "Got data when we didn't expect it!";
        return;
    }

    if(isConnected()) {
        responseData.append(serial->readAll());
    }

//    for(int i = 0; i < responseData.count(); i++)
//        qDebug() << "Data at " << i << ": "
//                 << (int)responseData.at(i)
//                 << "(" << responseData.at(i) << ")";

    if(responseData.length() > commandQueue.front().expectedResponse.length()) {
        // TODO: error, we got unexpected data.
        qCritical() << "Got more data than we expected";
        return;
    }

    if(responseData.length() < commandQueue.front().expectedResponse.length()) {
        qDebug() << "Didn't get enough data yet. Expecting:"
                 << commandQueue.front().expectedResponse.length()
                 << " received: " << responseData.length();
        return;
    }

    // If the command was to read from flash, short-circuit the response data check.
    if(commandQueue.front().name == "readFlash") {
        if(responseData.at(responseData.length()-1) != '\r') {
            qCritical() << "readFlash response didn't end with a \\r";
            return;
        }
    }
    // If the expected data should be masked, do a chracter by character match
    else if(commandQueue.front().expectedResponseMask.length() > 0) {
        if(commandQueue.front().expectedResponseMask.length() !=
            commandQueue.front().expectedResponse.length()) {
            qCritical() << "Expected response mask length incorrect!";
            return;
        }
        for(int i = 0; i < responseData.length(); i++) {
            if(commandQueue.front().expectedResponseMask.at(i) != 0
                && responseData[i] != commandQueue.front().expectedResponse.at(i)) {
                qCritical() << "Got unexpected data back"
                            << " position=" << i
                            << " expected=" << (int)responseData[i]
                            << " received=" << (int)commandQueue.front().expectedResponse.at(i)
                            << " mask=" << (int)commandQueue.front().expectedResponseMask.at(i);
                return;
            }
        }
    }
    // Otherwise just check if the strings match
    else if(responseData != commandQueue.front().expectedResponse) {
        qCritical() << "Got unexpected data back";
        return;
    }

//    qDebug() << "Command completed successfully: " << commandQueue.front().name;
    emit(commandFinished(commandQueue.front().name,responseData));


    // If the command was reset, disconnect from the programmer and cancel
    // any further commands
    if(commandQueue.front().name == "reset") {
        qDebug() << "Disconnecting from programmer";

        resetState();
        return;
    }

    // At this point, we've gotten all of the data that we expected.
    commandTimeoutTimer->stop();

    // TODO: There's some danger of an out-of-order operation here.
    // if processCommandQueue() is run by anything else before the command
    // is popped, the current command could be run twice.
    commandQueue.pop_front();

    // Start another command, if there is one.
    processCommandQueue();
}

void SerialCommandQueue::handleSerialError(QSerialPort::SerialPortError serialError)
{
    // If we aren't supposed to be connected, discard resource errors
    if(!serial->isOpen() && serialError == QSerialPort::ResourceError) {
        return;
    }

    if(serialError == QSerialPort::NoError) {
        // The serial library appears to emit an extraneous SerialPortError
        // when open() is called. Just ignore it.
        return;
    }

    emit(error(serial->errorString()));

    resetState();
}

void SerialCommandQueue::handleCommandTimeout()
{
    qCritical() << "Command timed out, disconnecting from programmer";
    emit(error("Command timed out, disconnecting from programmer"));

    resetState();
}

void SerialCommandQueue::resetState() {
    close();
    commandQueue.clear();
    responseData.clear();
}
