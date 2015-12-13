#ifndef BLINKYPENDANTCOMMANDQUEUE_H
#define BLINKYPENDANTCOMMANDQUEUE_H

#include "serialcommandqueue.h"


class BlinkyPendantCommandQueue : public SerialCommandQueue
{
    Q_OBJECT

public:
    BlinkyPendantCommandQueue(QObject *parent = 0);

    void startWrite();

    void writeData(QByteArray& data);

    void stopWrite();
};

#endif // BLINKYPENDANTCOMMANDQUEUE_H
