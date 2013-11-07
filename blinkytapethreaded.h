#ifndef BLINKYTAPETHREADED_H
#define BLINKYTAPETHREADED_H

#include <QThread>
#include "blinkytape.h"

// Basically all this class does is manage a BlinkyTape that's in a separate thread
class BlinkyTapeThreaded : public QObject
{
    Q_OBJECT
public:
    explicit BlinkyTapeThreaded(QObject *parent = 0);
    ~BlinkyTapeThreaded();

    static QList<QSerialPortInfo> findBlinkyTapes();

    bool tapeConnect(QSerialPortInfo info);
    void tapeDisconnect();

    void uploadAnimation(QByteArray animation, int frameRate);

    bool isConnected();

    void sendUpdate(QByteArray colors);

private:
    QThread* thread;
    BlinkyTape* tape;

    bool connected; // TODO: poll this in real time instead of caching it?

signals:
    void connectionStatusChanged(bool status);

public slots:
    // TODO: These slots should be private?
    void relayConnectionStatusChanged(bool status);

};

#endif // BLINKYTAPETHREADED_H
