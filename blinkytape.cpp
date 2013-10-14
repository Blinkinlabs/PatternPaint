#include "blinkytape.h"

#include <iostream>

QList<QSerialPortInfo> BlinkyTape::findBlinkyTapes() {
    QList<QSerialPortInfo> tapes;

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        // If the description is BlinkyTape, we're sure to have a match
        if(info.description().startsWith("BlinkyTape")) {
            tapes.push_back(info);
        }
        // If it's a leonardo, it /may/ be a BlinkyTape running a user sketch
        else if(info.description().startsWith("Arduino Leonardo")) {
            tapes.push_back(info);
        }
    }

    return tapes;
}

BlinkyTape::BlinkyTape(int ledCount)
{
    m_ledCount = ledCount;
}

bool BlinkyTape::isConnected() {
    return m_serial.isOpen();
}

bool BlinkyTape::connect(QSerialPortInfo info) {
    // TODO: Refuse if we are already open?

    m_serial.setPort(info);

    // TODO: Do something else if we can't open?
    return m_serial.open(QIODevice::ReadWrite);
}

void BlinkyTape::disconnect() {
    if(isConnected()) {
        m_serial.close();
    }
}

void BlinkyTape::sendUpdate(QByteArray LedData)
{
    if(!isConnected()) {
        // TODO: Signal error?
        return;
    }

    // Kill anything that's 0xff
    // TODO: Don't do this in-place
    for(int i = 0; i < LedData.length(); i++) {
        if(LedData[i] == (char)255) {
            LedData[i] = 254;
        }
    }

    // Chunk it out
    int CHUNK_SIZE = 600;
    for(int p = 0; p < m_ledCount * 3; p += CHUNK_SIZE) {
        int length = 0;
        if(p + CHUNK_SIZE < LedData.length()) {
            // send a whole chunk
            length = CHUNK_SIZE;
        }
        else {
            // send a partial chunk
            length = LedData.length() - p;
        }

        QByteArray chunk(length, 0);
        for(int i = 0; i < length; i++) {
            chunk[i] = LedData[p + i];
        }

//        std::cout << "writing out" << std::endl;
        int written = m_serial.write(chunk);
        // if we didn't write everything, save it for later.
        if(written == -1) {
            exit(100);
            p -= CHUNK_SIZE;
        }
        else if(written != length) {
            exit(101);
            p-= length - written;
        }
//        std::cout << "waiting for write" << std::endl;
        m_serial.flush();
//        std::cout << "done waiting for write" << std::endl;
    }

    // Then send the flip command
    QByteArray data(3, 0);
    data[0] = 0xff;
    data[1] = 0xff;
    data[2] = 0xff;

//    std::cout << "writing end" << std::endl;
    m_serial.write(data);
//    std::cout << "waiting for end" << std::endl;
    m_serial.flush();
//    std::cout << "done waiting for end" << std::endl;
}
