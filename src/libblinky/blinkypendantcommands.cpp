#include "blinkypendantcommands.h"

#include <QDebug>

#include "bytearrayhelpers.h"

#define COMMAND_START_WRITE 0x01
#define COMMAND_WRITE       0x02
#define COMMAND_STOP_WRITE  0x03

#define PAGE_SIZE_BYTES     1024    // Size unit for writes to flash
#define CHUNK_SIZE_BYTES    64      // Number of bytes the write command requires per call

namespace BlinkyPendantCommands {

QByteArray commandHeader()
{
    return QByteArray(10, (char)0xFF);
}

SerialCommand startWrite()
{
    QByteArray command;
    command += commandHeader();

    command.append((char)COMMAND_START_WRITE);

    QByteArray response;
    response.append('P');
    response.append((char)0x00);
    response.append((char)0x00);

    QByteArray responseMask;
    responseMask.append((char)0xFF);
    responseMask.append((char)0xFF);
    responseMask.append((char)0x00);

    return SerialCommand("startWrite", command, response, responseMask, 2000);
}

SerialCommand stopWrite()
{
    QByteArray command;
    command += commandHeader();

    command.append(COMMAND_STOP_WRITE);

    QByteArray response;
    response.append('P');
    response.append((char)0x00);
    response.append((char)0x00);

    QByteArray responseMask;
    responseMask.append((char)0xFF);
    responseMask.append((char)0xFF);
    responseMask.append((char)0x00);

    return SerialCommand("stopWrite", command, response, responseMask);
}

SerialCommand writeFlashChunk(const QByteArray &data)
{
    // TODO: How to error out here?
    if(data.length() != CHUNK_SIZE_BYTES)
        qCritical() << "Bad data size, padding"
                    << "got:" << data.length()
                    << "expected:" << CHUNK_SIZE_BYTES;

    QByteArray paddedData(data.mid(0,CHUNK_SIZE_BYTES));

    while (paddedData.length() < CHUNK_SIZE_BYTES)
        paddedData.append((char)0xFF);

    QByteArray command;
    command += commandHeader();

    command.append(COMMAND_WRITE);
    command.append(paddedData);

    QByteArray response;
    response.append('P');
    response.append((char)0x00);
    response.append((char)0x00);

    QByteArray responseMask;
    responseMask.append((char)0xFF);
    responseMask.append((char)0xFF);
    responseMask.append((char)0x00);

    return SerialCommand("writeFlashPage", command, response, responseMask);
}

QList<SerialCommand> writeFlash(const QByteArray &data)
{
    QByteArray paddedData(data);
    QList<SerialCommand> commands;

    // Pad the data to a page boundary
    while (paddedData.length() % PAGE_SIZE_BYTES != 0)
        paddedData.append((char)0xFF);

    for (QByteArray chunk : ByteArrayCommands::chunkData(paddedData, CHUNK_SIZE_BYTES))
        commands.append(writeFlashChunk(chunk));

    return commands;
}

}
