#ifndef LIGHTBUDDYSERIALQUEUE_H
#define LIGHTBUDDYSERIALQUEUE_H

#include <QObject>
#include "serialcommandqueue.h"

class LightBuddySerialQueue : public SerialCommandQueue
{
    Q_OBJECT

public:
    LightBuddySerialQueue(QObject *parent = 0);

    void eraseFlash();
    void fileNew(int sizeBytes);
    void writePage(int sector, int offset, QByteArray data);
    void reloadAnimations();
};

#endif // LIGHTBUDDYSERIALQUEUE_H
