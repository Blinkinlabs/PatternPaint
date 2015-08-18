#ifndef LIGHTBUDDYPROGRAMMER_H
#define LIGHTBUDDYPROGRAMMER_H

#include <QObject>
#include "serialcommandqueue.h"

class LightBuddyProgrammer : public SerialCommandQueue
{
    Q_OBJECT

public:
    LightBuddyProgrammer(QObject *parent = 0);

    void eraseFlash();
    void fileNew(int sizeBytes);
    void writePage(int sector, int offset, QByteArray data);
    void reloadAnimations();

};

#endif // LIGHTBUDDYPROGRAMMER_H
