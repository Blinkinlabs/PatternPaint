#ifndef BLINKYTAPE_H
#define BLINKYTAPE_H

#include <QList>
#include <QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>

/// Connect to a BlinkyTape over a serial port, and manage sending data to it.
class BlinkyTape
{
public:
    static QList<QSerialPortInfo> findBlinkyTapes();


    BlinkyTape(int ledCount = 60);

    bool isConnected();

    bool connect(QSerialPortInfo info);

    void disconnect();

    void sendUpdate(QByteArray colors);

private:
    QSerialPort m_serial;

    int m_ledCount;
};

#endif // BLINKYTAPE_H
