#include "blinkytape.h"

#include <iostream>

QList<QSerialPortInfo> BlinkyTape::findBlinkyTapes() {
    QList<QSerialPortInfo> tapes;

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        // If the description is BlinkyTape, we're sure to have a match
        if(info.description().startsWith("BlinkyTape")) {
            tapes.push_back(info);
        }

        // If it's a leonardo, though, it becomes a little unclear
        // TODO: Leonard support: just accept them all?
    }

    return tapes;
}

BlinkyTape::BlinkyTape(int ledCount)
{
    m_ledCount = ledCount;
}


bool BlinkyTape::connect(QSerialPortInfo info) {
    m_serial.setPort(info);

    // TODO: Do something else if we can't open?
    return m_serial.open(QIODevice::ReadWrite);
}

void BlinkyTape::sendUpdate(QByteArray LedData)
{
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

        std::cout << "writing out" << std::endl;
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
        std::cout << "waiting for write" << std::endl;
        m_serial.flush();
        std::cout << "done waiting for write" << std::endl;

//                        std::cout << "p:" << p << " length:" << length << std::endl;
    }

    // Then send the flip command
    QByteArray data(3, 0);
    data[0] = 0xff;
    data[1] = 0xff;
    data[2] = 0xff;

    std::cout << "writing end" << std::endl;
    m_serial.write(data);
    std::cout << "waiting for end" << std::endl;
    m_serial.flush();
    std::cout << "done waiting for end" << std::endl;
}
