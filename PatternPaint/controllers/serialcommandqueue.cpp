#include "serialcommandqueue.h"

SerialCommandQueue::SerialCommandQueue(QObject *parent) : QObject(parent)
{
    serial = new QSerialPort(this);
    serial->setSettingsRestoredOnClose(false);

    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)),
            this, SLOT(handleSerialError(QSerialPort::SerialPortError)));
    connect(serial, SIGNAL(readyRead()), this, SLOT(handleReadData()));

    commandTimeoutTimer.setSingleShot(true);

    connect(&commandTimeoutTimer, SIGNAL(timeout()),
            this, SLOT(handleCommandTimeout()));
}

bool SerialCommandQueue::open(QSerialPortInfo info)
{
    if (isConnected()) {
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

    if (!serial->open(QIODevice::ReadWrite)) {
        qDebug() << "Could not connect to serial device. Error: " << serial->error()
                 << serial->errorString();
        return false;
    }

    return true;
}

void SerialCommandQueue::close()
{
    if (serial->isOpen())
        serial->close();

    queue.clear();
    responseData.clear();

    commandTimeoutTimer.stop();
}

bool SerialCommandQueue::isConnected()
{
    return serial->isOpen();
}

void SerialCommandQueue::enqueue(QList<SerialCommand> commands)
{
    foreach (SerialCommand command, commands)
        enqueue(command);
}

void SerialCommandQueue::enqueue(SerialCommand command)
{
// qDebug() << "queuing command:" << command.name
// << "length:" << command.data.count();

    if (command.expectedResponseMask.count() > 0) {
        if (command.expectedResponse.count() != command.expectedResponseMask.count()) {
            qCritical() << "Expected response mask length incorrect!";
            emit(error("Expected response mask length incorrect!"));
            return;
        }
    }

    queue.push_back(command);

    // Try to start processing commands.
    processQueue();
}

void SerialCommandQueue::processQueue()
{
    // Nothing to do if we don't have any commands...
    if (queue.count() == 0)
        return;

    // Don't start a new command if one is already in progress
    if (commandTimeoutTimer.isActive())
        return;

    if (!isConnected()) {
        qCritical() << "Device disappeared, cannot run command";
        return;
    }

    //qDebug() << "Starting Command:" << queue.front().name;
    responseData.clear();

    if (serial->write(queue.front().data)
        != queue.front().data.length()) {
        qCritical() << "Error writing to device";
        return;
    }

    // Start the timer; the command must complete before it fires, or it
    // is considered an error. This is to prevent a misbehaving device from hanging
    // the programmer code.
    commandTimeoutTimer.start(queue.front().timeout);
}

void SerialCommandQueue::handleReadData()
{
    if (queue.length() == 0) {
        // TODO: error, we got unexpected data.
        qCritical() << "Got data when we didn't expect it!";
        return;
    }

    if (isConnected())
        responseData.append(serial->readAll());

// for(int i = 0; i < responseData.count(); i++)
// qDebug() << "Data at " << i << ": "
// << (int)responseData.at(i)
// << "(" << responseData.at(i) << ")";

    if (responseData.length() > queue.front().expectedResponse.length()) {
        // TODO: error, we got unexpected data.
        qCritical() << "Got more data than we expected";
        return;
    }

    if (responseData.length() < queue.front().expectedResponse.length()) {
        qDebug() << "Didn't get enough data yet. Expecting:"
                 << queue.front().expectedResponse.length()
                 << " received: " << responseData.length();
        return;
    }

    // If the expected data should be masked, do a chracter by character match
    if (queue.front().expectedResponseMask.length() > 0) {
        for (int i = 0; i < responseData.length(); i++) {
            if (queue.front().expectedResponseMask.at(i) != 0
                && responseData[i] != queue.front().expectedResponse.at(i)) {
                qCritical() << "Got unexpected data back"
                            << " position=" << i
                            << " expected=" << (int)queue.front().expectedResponse.at(i)
                            << " received=" << (int)responseData[i]
                            << " mask=" << (int)queue.front().expectedResponseMask.at(i);
                emit(error("Got unexpected data back"));
                return;
            }
        }
    }
    // Otherwise just check if the strings match
    else if (responseData != queue.front().expectedResponse) {
        qCritical() << "Got unexpected data back";
        emit(error("Got unexpected data back"));
        return;
    }

// qDebug() << "Command completed successfully: " << commandQueue.front().name;

    QMetaObject::invokeMethod(this, "commandFinished", Qt::QueuedConnection,
                              Q_ARG(QString, queue.front().name), Q_ARG(QByteArray, responseData));

    // At this point, we've gotten all of the data that we expected.
    commandTimeoutTimer.stop();

    queue.pop_front();

    // Start another command, if there is one.
    processQueue();
}

void SerialCommandQueue::handleSerialError(QSerialPort::SerialPortError serialError)
{
    // If we aren't supposed to be connected, discard resource errors
    if (!serial->isOpen() && serialError == QSerialPort::ResourceError)
        return;

    if (serialError == QSerialPort::NoError) {
        // The serial library appears to emit an extraneous SerialPortError
        // when open() is called. Just ignore it.
        return;
    }

    emit(error(serial->errorString()));

    close();
}

void SerialCommandQueue::handleCommandTimeout()
{
    qCritical() << "Command " << queue.front().name
                << " timed out, disconnecting from programmer";

    emit(error("Command timed out, disconnecting from programmer"));

    close();
}
