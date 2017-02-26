#ifndef FIRMWAREIMPORT_H
#define FIRMWAREIMPORT_H

#include "memorysection.h"

#include <QString>

bool parseHexLine(QString line,
                  unsigned int &address,
                  unsigned int &type,
                  QByteArray &data);

class FirmwareReader
{
public:
    bool load(const QString &fileName);

    const MemorySection &getData() const;

private:
    MemorySection data;
};

#endif // FIRMWAREIMPORT_H
