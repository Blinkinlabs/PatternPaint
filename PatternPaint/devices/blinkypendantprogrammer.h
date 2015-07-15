#ifndef BLINKYPENDANTPROGRAMMER_H
#define BLINKYPENDANTPROGRAMMER_H

#include "serialcommandqueue.h"


class BlinkyPendantProgrammer : public SerialCommandQueue
{
    Q_OBJECT

public:
    BlinkyPendantProgrammer(QObject *parent = 0);

    void startWrite();

    void writeData(QByteArray& data);

    void stopWrite();
};

#endif // BLINKYPENDANTPROGRAMMER_H
