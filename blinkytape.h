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

    bool isConnected();

    void open(QSerialPortInfo info);
    void close();

    void sendUpdate(QByteArray colors);

    // Attempt to upload a new animation to the tape
    void uploadAnimation(QByteArray animation, int frameRate);

private:
    // TODO: Destructor!
    QSerialPort* serial;
    int ledCount;

signals:
    void connectionStatusChanged(bool status);

private slots:
    void handleError(QSerialPort::SerialPortError error);

};

#endif // BLINKYTAPE_H
