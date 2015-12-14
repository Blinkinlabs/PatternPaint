#include "blinkypendantcommands.h"

#include <QDebug>

#define COMMAND_START_WRITE 0x01
#define COMMAND_WRITE       0x02
#define COMMAND_STOP_WRITE  0x03

#define PAGE_SIZE_BYTES     1024    // Size unit for writes to flash
#define WRITE_SIZE_BYTES    64      // Number of bytes the write command requires per call

namespace BlinkyPendantCommands {

QByteArray commandHeader()
{
    QByteArray output;
    for (int i = 0; i < 10; i++)
        output.append((char)0xFF);
    return output;
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

SerialCommand writeFlashPage(QByteArray data)
{
    QByteArray command;
    command += commandHeader();

    command.append(COMMAND_WRITE);

    command += data;

    QByteArray response;
    response.append('P');
    response.append((char)0x00);
    response.append((char)0x00);

    QByteArray responseMask;
    responseMask.append((char)0xFF);
    responseMask.append((char)0xFF);
    responseMask.append((char)0x00);

    return SerialCommand("writePage", command, response, responseMask);
}

QList<SerialCommand> writeData(QByteArray &data)
{
    QList<SerialCommand> commands;

    if (data.length() == 0) {
        // TODO: How to report an error here?
        qCritical() << "No data to write";
        return commands;
    }

    // Pad the data to the page size
    while (data.length()%PAGE_SIZE_BYTES != 0)
        data.append((char)0xFF);

    for (int currentChunkPosition = 0;
         currentChunkPosition < data.length();
         currentChunkPosition += WRITE_SIZE_BYTES) {

        commands.append(writeFlashPage(data.mid(currentChunkPosition, WRITE_SIZE_BYTES)));
    }

    return commands;
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
}
