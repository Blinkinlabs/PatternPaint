#ifndef BLINKYTAPE_H
#define BLINKYTAPE_H

#include <QObject>
#include <QList>
#include <QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>

/// Connect to a BlinkyTape over a serial port, and manage sending data to it.
class BlinkyTape : public QObject
{
    Q_OBJECT
public:
    static QList<QSerialPortInfo> findBlinkyTapes();

    BlinkyTape(int ledCount = 60);

    bool connect(QSerialPortInfo info);
    void disconnect();

    // Attempt to upload a new animation to the tape
    void uploadAnimation(QByteArray animation, int frameRate);

    bool isConnected();

    void sendUpdate(QByteArray colors);

private:
    QSerialPort serial;

    int ledCount;

signals:
   void connectionStatusChanged(bool status);
};

#endif // BLINKYTAPE_H
