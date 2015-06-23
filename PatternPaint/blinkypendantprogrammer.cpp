#include "blinkypendantprogrammer.h"

#define COMMAND_START_WRITE 0x01
#define COMMAND_WRITE       0x02
#define COMMAND_STOP_WRITE  0x03

#define PAGE_SIZE_BYTES     1024    // Size unit for writes to flash
#define WRITE_SIZE_BYTES    64      // Number of bytes the write command requires per call

BlinkyPendantProgrammer::BlinkyPendantProgrammer(QObject *parent) :
    SerialCommandQueue(parent)
{

}

void BlinkyPendantProgrammer::startWrite() {
    QByteArray command;
    for(int i = 0; i < 10; i++) {
        command.append(0xFF);
    }
    command.append(COMMAND_START_WRITE);

    QByteArray response;
    response.append('P');
    response.append((char)0);
    response.append((char)0);

    QByteArray responseMask;
    responseMask.append((char)1);
    responseMask.append((char)0);
    responseMask.append((char)0);

    queueCommand("startWrite",command, response, responseMask);
}

void BlinkyPendantProgrammer::writeData(QByteArray& data) {
    if(data.length() == 0) {
        qCritical() << "No data to write";
        return;
    }

    // Pad the data to the page size
    while(data.length()%PAGE_SIZE_BYTES != 0) {
        data.append((char)0xFF);
    }

    for(int currentChunkPosition = 0;
        currentChunkPosition < data.length();
        currentChunkPosition += WRITE_SIZE_BYTES) {

        QByteArray command;
        for(int i = 0; i < 10; i++) {
            command.append(0xFF);
        }
        command.append(COMMAND_WRITE);

        command += data.mid(currentChunkPosition, WRITE_SIZE_BYTES);

        QByteArray response;
        response.append('P');
        response.append((char)0);
        response.append((char)0);

        QByteArray responseMask;
        responseMask.append((char)1);
        responseMask.append((char)0);
        responseMask.append((char)0);

        queueCommand("write",command, response, responseMask);
    }
}

void BlinkyPendantProgrammer::stopWrite() {
    QByteArray command;
    for(int i = 0; i < 10; i++) {
        command.append(0xFF);
    }
    command.append(COMMAND_STOP_WRITE);

    QByteArray response;
    response.append('P');
    response.append((char)0);
    response.append((char)0);

    QByteArray responseMask;
    responseMask.append((char)1);
    responseMask.append((char)0);
    responseMask.append((char)0);

    queueCommand("stopWrite",command, response, responseMask);
}
