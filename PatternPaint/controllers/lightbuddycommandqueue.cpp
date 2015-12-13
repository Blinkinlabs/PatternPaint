#include "lightbuddycommandqueue.h"


void addHeader(QByteArray& command) {
    for(int i = 0; i < 12; i++) {
        command.append((char)0xFF);
    }
}

LightBuddySerialQueue::LightBuddySerialQueue(QObject *parent) :
    SerialCommandQueue(parent)
{

}

void LightBuddySerialQueue::eraseFlash() {
    QByteArray command;
    addHeader(command);

    command.append((char)0x20);
    command.append('E');
    command.append('e');

    QByteArray ret;
    ret.append('P');
    ret.append((char)0x00);
    ret.append((char)0x00);

    QByteArray mask;
    mask.append((char)0xFF);
    mask.append((char)0x00);
    mask.append((char)0x00);

    // Note: only the length matters for the response, the response data
    // will be ignored.
    queueCommand("eraseFlash",command,ret,mask);
}

void LightBuddySerialQueue::largestFile()
{
    QByteArray command;
    addHeader(command);

    command.append((char)0x11);

    QByteArray ret;
    ret.append('P');
    ret.append((char)0x03);
    ret.append((char)0x00);
    ret.append((char)0x00);
    ret.append((char)0x00);
    ret.append((char)0x00);

    QByteArray mask;
    mask.append((char)0xFF);
    mask.append((char)0xFF);
    mask.append((char)0x00);
    mask.append((char)0x00);
    mask.append((char)0x00);
    mask.append((char)0x00);

    // Note: only the length matters for the response, the response data
    // will be ignored.
    queueCommand("largestFile",command,ret,mask);
}

void LightBuddySerialQueue::fileNew(int sizeBytes) {

    QByteArray command;
    addHeader(command);

    command.append((char)0x18);   // New file
    command.append((char)0x12);   // filetype = animation
    command.append((char)((sizeBytes >> 24) & 0xFF));
    command.append((char)((sizeBytes >> 16) & 0xFF));
    command.append((char)((sizeBytes >>  8) & 0xFF));
    command.append((char)((sizeBytes      ) & 0xFF));

    QByteArray ret;
    ret.append('P');
    ret.append((char)0x03);
    ret.append((char)0x00);
    ret.append((char)0x00);
    ret.append((char)0x00);
    ret.append((char)0x00);

    QByteArray mask;
    mask.append((char)0xFF);
    mask.append((char)0xFF);
    mask.append((char)0x00);
    mask.append((char)0x00);
    mask.append((char)0x00);
    mask.append((char)0x00);

//    // We're expecting 4 bytes of sector data back
//        sector = 0
//        if status:
//            sector += ord(returnData[0]) << 24
//            sector += ord(returnData[1]) << 16
//            sector += ord(returnData[2]) << 8
//            sector += ord(returnData[3]) << 0

    queueCommand("fileNew",command,ret,mask);
}

void LightBuddySerialQueue::writePage(int sector, int offset, QByteArray data) {
    if(data.size() != 256) {
        // TODO: How to error out here?
        return;
    }

    QByteArray command;
    addHeader(command);

    command.append((char)0x19);
    command.append((char)((sector >> 24) & 0xFF));
    command.append((char)((sector >> 16) & 0xFF));
    command.append((char)((sector >>  8) & 0xFF));
    command.append((char)((sector      ) & 0xFF));
    command.append((char)((offset >> 24) & 0xFF));
    command.append((char)((offset >> 16) & 0xFF));
    command.append((char)((offset >>  8) & 0xFF));
    command.append((char)((offset      ) & 0xFF));
    command += data;

    QByteArray ret;
    ret.append('P');
    ret.append((char)0x00);
    ret.append((char)0x00);

    QByteArray mask;
    mask.append((char)0xFF);
    mask.append((char)0xFF);
    mask.append((char)0x00);

    // Note: only the length matters for the response, the response data
    // will be ignored.
    queueCommand("writePage",command,ret, mask);
}

void LightBuddySerialQueue::reloadAnimations() {

    QByteArray command;
    addHeader(command);
    command.append((char)0x02);

    QByteArray ret;
    ret.append('P');
    ret.append((char)0x00);
    ret.append((char)0x00);

    QByteArray mask;
    mask.append((char)0xFF);
    mask.append((char)0xFF);
    mask.append((char)0x00);

    // Note: only the length matters for the response, the response data
    // will be ignored.
    queueCommand("reloadAnimations",command,ret,mask);
}
