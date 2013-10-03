#ifndef BLINKYTAPE_H
#define BLINKYTAPE_H

#include <QList>
#include <QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class BlinkyTape
{
public:
    static QList<QSerialPortInfo> findBlinkyTapes();

private:
    QSerialPort m_serial;

    int m_ledCount;

public:
    BlinkyTape(int ledCount = 60);

    bool connect(QSerialPortInfo info);

//    void disconnect();

    void sendUpdate(QByteArray colors);
};

#endif // BLINKYTAPE_H
