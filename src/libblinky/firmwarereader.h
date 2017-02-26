#ifndef FIRMWAREIMPORT_H
#define FIRMWAREIMPORT_H

#include <QString>

bool parseHexLine(QString line,
                  unsigned int &address,
                  unsigned int &type,
                  QByteArray &data);

class FirmwareReader
{
public:
    bool load(const QString &fileName);

    const QByteArray &getData() const;

private:
    QByteArray data;
};

#endif // FIRMWAREIMPORT_H
