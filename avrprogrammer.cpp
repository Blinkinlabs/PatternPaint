#include "avrprogrammer.h"
#include <iostream>

AvrProgrammer::AvrProgrammer()
{
}

bool AvrProgrammer::connect(QSerialPortInfo info) {
    // TODO: Refuse if we are already open?

    m_serial.setPort(info);

    // TODO: Do something else if we can't open?
    return m_serial.open(QIODevice::ReadWrite);
}

void AvrProgrammer::disconnect() {
    if(isConnected()) {
        m_serial.close();
    }
}

bool AvrProgrammer::isConnected() {
    return m_serial.isOpen();
}

void AvrProgrammer::enterProgrammingMode() {
    QByteArray a;
    a.append('T');
    a.append(0x64);

    m_serial.write(a);
    m_serial.flush();

    QByteArray returnData;
    returnData = m_serial.read(1);
    if(returnData.length() != 1) {
        // TODO: Read error, bail!
        std::cout << "Fuck, didn't get a byte back!" << std::endl;
    }
    if( returnData.at(0) != '\r') {
        // TODO: Return code invalid, bail!
        std::cout << "Fuck, didn't get a CR back, got:" << (int)returnData.at(0) << std::endl;
    }
}

void AvrProgrammer::leaveProgrammingMode() {

}
