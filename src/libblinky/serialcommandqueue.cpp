#include "serialcommandqueue.h"

SerialCommandQueue::SerialCommandQueue(QObject *parent) :
    QObject(parent)
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

    qDebug() << "connecting to" << info.portName();

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
    return (serial != NULL && serial->isOpen());
}

void SerialCommandQueue::enqueue(const QList<SerialCommand> &commands)
{
    for (const SerialCommand &command : commands)
        enqueue(command);
}

void SerialCommandQueue::flushQueue()
{
    // TODO: this is dangerous, if a command is already running it could
    // put the device in a bad state. Switch to some version that only takes
    // effect between command executions
    queue.clear();
    responseData.clear();

    commandTimeoutTimer.stop();
}

int SerialCommandQueue::length()
{
    return queue.length();
}

void SerialCommandQueue::enqueue(const SerialCommand &command)
{
// qDebug() << "queuing command:" << command.name
// << "length:" << command.data.count();

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

//    qDebug() << "Starting Command:" << queue.front().name;
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
    if (!isConnected())
        return;

    responseData.append(serial->readAll());

//    qDebug() << "Got data: length:" << responseData.length()
//             << "data: " << responseData.toHex();

    if (queue.length() == 0) {
        // TODO: error, we got unexpected data.
        qCritical() << "Got data when we didn't expect it!";
        return;
    }

    switch(queue.front().testResponse(responseData)) {
    case RESPONSE_NOT_ENOUGH_DATA:
        qDebug() << "Didn't get enough data yet. Expecting:"
                 << queue.front().expectedResponse.length()
                 << "received:" << responseData.length();
        break;

    case RESPONSE_TOO_MUCH_DATA:
        // TODO: error, we got unexpected data.
        qCritical() << "Got more data than we expected"
                    << "expected:" << queue.front().expectedResponse.length()
                    << "received:" << responseData.length();
        break ;

    case RESPONSE_INVALID_MASK:
        // TODO: error, we got unexpected data.
        qCritical() << "Invalid mask length- command formatted incorrectly."
                    << "expectedResponse:" << queue.front().expectedResponse.length()
                    << "expectedResponseMask:" << queue.front().expectedResponseMask.length();
        break;

    case RESPONSE_MISMATCH:
        emit(error("Got unexpected data back"));
        break;

    case RESPONSE_MATCH:

        // qDebug() << "Command completed successfully: " << commandQueue.front().name;

        QMetaObject::invokeMethod(this, "commandFinished", Qt::QueuedConnection,
                                  Q_ARG(QString, queue.front().name), Q_ARG(QByteArray, responseData));

        // At this point, we've gotten all of the data that we expected.
        commandTimeoutTimer.stop();

        queue.pop_front();

        // If that was the last command, signal it
        if(queue.empty()) {
            QMetaObject::invokeMethod(this, "lastCommandFinished", Qt::QueuedConnection);
        }

        // Start another command, if there is one.
        processQueue();

        break;
    }
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
    QString errorMessage = QString("Command %1 timed out").arg((queue.front().name));

    qCritical() << errorMessage;

    emit(error(errorMessage));

    // TODO: Does skipping this break behavior anywhere?
//    close();
}
