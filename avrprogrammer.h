#ifndef AVRPROGRAMMER_H
#define AVRPROGRAMMER_H

#include <QtSerialPort>


// Given a serial object that is suspected to have an AVR109-compatible booloader
// attached to it (ie, caterina), use it to load a new user program.
//
// Note that these are just commands; you still need to send them in the
// correct sequence.
//
// Based on AVRProgrammer from AVR911 - AVR Open-source Programmer
// https://code.google.com/p/avrosp/source/browse/trunk/SourceCode/AVRInSystemProg.hpp
class AvrProgrammer
{
public:
    AvrProgrammer();

    bool connect(QSerialPortInfo info);
    void disconnect();

    bool isConnected();

    void enterProgrammingMode();
    void leaveProgrammingMode();

//    bool chipErase();

//    bool readOSCCAL( long pos, long * value );
//    bool readSignature( long * sig0, long * sig1, long * sig2 );
//    bool checkSignature( long sig0, long sig1, long sig2 );

//    bool writeFlashByte( long address, long value );
//    bool writeEEPROMByte( long address, long value );

//    bool writeFlash( HEXFile * data );
//    bool readFlash( HEXFile * data );

//    bool writeEEPROM( HEXFile * data );
//    bool readEEPROM( HEXFile * data );

//    bool writeLockBits( long bits );
//    bool readLockBits( long * bits );

//    bool writeFuseBits( long bits );
//    bool readFuseBits( long * bits );
//    bool writeExtendedFuseBits( long bits );
//    bool readExtendedFuseBits( long * bits );

//    bool programmerSoftwareVersion( long * major, long * minor );
//    bool programmerHardwareVersion( long * major, long * minor );

private:
    QSerialPort serial;

    // Send a command to a connected bootloader
    bool sendCommand(QByteArray command);

    // Check that a known response was received
    bool checkResponse(QByteArray response);

    // Check that we are talking to the correct bootloader
    bool checkSoftwareIdentifier();
};

#endif // AVRPROGRAMMER_H
