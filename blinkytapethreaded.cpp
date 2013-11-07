#include "blinkytapethreaded.h"

#include <iostream>

BlinkyTapeThreaded::BlinkyTapeThreaded(QObject *parent)
{
    // TODO: Start this later if possible?
    thread = new QThread;
    thread->setObjectName("BlinkyTapeThreaded");

    tape = new BlinkyTape();
    tape->moveToThread(thread);

    connect(tape, SIGNAL(connectionStatusChanged(bool)), this, SLOT(relayConnectionStatusChanged(bool)));

    thread->start();
}

BlinkyTapeThreaded::~BlinkyTapeThreaded() {

}

QList<QSerialPortInfo> BlinkyTapeThreaded::findBlinkyTapes()
{
    return BlinkyTape::findBlinkyTapes();
}


bool BlinkyTapeThreaded::tapeConnect(QSerialPortInfo info) {
    tape->open(info);
    return false;
}

void BlinkyTapeThreaded::tapeDisconnect() {
    tape->close();
}

void BlinkyTapeThreaded::uploadAnimation(QByteArray animation, int frameRate) {
    tape->uploadAnimation(animation, frameRate);
}

bool BlinkyTapeThreaded::isConnected() {
    return connected;
}

void BlinkyTapeThreaded::sendUpdate(QByteArray colors) {
    tape->sendUpdate(colors);
}

void BlinkyTapeThreaded::relayConnectionStatusChanged(bool status) {
    std::cout << "Connection status changed!" << status << std::endl;
    connected = status;
    emit(connectionStatusChanged(status));
}
