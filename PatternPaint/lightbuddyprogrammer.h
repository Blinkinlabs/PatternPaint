#ifndef LIGHTBUDDYPROGRAMMER_H
#define LIGHTBUDDYPROGRAMMER_H

#include <QObject>
#include "serialcommandqueue.h"

class LightBuddyProgrammer : public SerialCommandQueue
{
    Q_OBJECT

public:
    LightBuddyProgrammer(QObject *parent = 0);

};

#endif // LIGHTBUDDYPROGRAMMER_H
