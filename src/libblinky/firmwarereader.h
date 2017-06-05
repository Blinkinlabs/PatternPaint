#ifndef FIRMWAREIMPORT_H
#define FIRMWAREIMPORT_H

#include "libblinkyglobal.h"

#include "memorysection.h"

#include <QString>

LIBBLINKY_EXPORT bool parseHexLine(QString line,
                  unsigned int &address,
                  unsigned int &type,
                  QByteArray &data);

class LIBBLINKY_EXPORT FirmwareReader
{
public:
    bool load(const QString &fileName);

    const MemorySection &getData() const;

private:
    MemorySection data;
};

#endif // FIRMWAREIMPORT_H
