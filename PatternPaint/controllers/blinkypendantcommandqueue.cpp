#include "blinkypendantcommandqueue.h"

#define COMMAND_START_WRITE 0x01
#define COMMAND_WRITE       0x02
#define COMMAND_STOP_WRITE  0x03

#define PAGE_SIZE_BYTES     1024    // Size unit for writes to flash
#define WRITE_SIZE_BYTES    64      // Number of bytes the write command requires per call

BlinkyPendantCommandQueue::BlinkyPendantCommandQueue(QObject *parent) :
    SerialCommandQueue(parent)
{
}

void BlinkyPendantCommandQueue::startWrite()
{
    QByteArray command;
    for (int i = 0; i < 10; i++)
        command.append((char)0xFF);
    command.append((char)COMMAND_START_WRITE);

    QByteArray response;
    response.append('P');
    response.append((char)0x00);
    response.append((char)0x00);

    QByteArray responseMask;
    responseMask.append((char)0xFF);
    responseMask.append((char)0xFF);
    responseMask.append((char)0x00);

    enqueue(SerialCommand("startWrite", command, response, responseMask));
}

void BlinkyPendantCommandQueue::writeData(QByteArray &data)
{
    if (data.length() == 0) {
        qCritical() << "No data to write";
        return;
    }

    // Pad the data to the page size
    while (data.length()%PAGE_SIZE_BYTES != 0)
        data.append((char)0xFF);

    for (int currentChunkPosition = 0;
         currentChunkPosition < data.length();
         currentChunkPosition += WRITE_SIZE_BYTES) {
        QByteArray command;
        for (int i = 0; i < 10; i++)
            command.append(0xFF);
        command.append(COMMAND_WRITE);

        command += data.mid(currentChunkPosition, WRITE_SIZE_BYTES);

        QByteArray response;
        response.append('P');
        response.append((char)0x00);
        response.append((char)0x00);

        QByteArray responseMask;
        responseMask.append((char)0xFF);
        responseMask.append((char)0xFF);
        responseMask.append((char)0x00);

        enqueue(SerialCommand("write", command, response, responseMask));
    }
}

void BlinkyPendantCommandQueue::stopWrite()
{
    QByteArray command;
    for (int i = 0; i < 10; i++)
        command.append(0xFF);
    command.append(COMMAND_STOP_WRITE);

    QByteArray response;
    response.append('P');
    response.append((char)0x00);
    response.append((char)0x00);

    QByteArray responseMask;
    responseMask.append((char)0xFF);
    responseMask.append((char)0xFF);
    responseMask.append((char)0x00);

    enqueue(SerialCommand("stopWrite", command, response, responseMask));
}
