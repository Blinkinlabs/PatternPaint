#ifndef BYTEARRAYHELPERS_H
#define BYTEARRAYHELPERS_H

#include <QByteArray>
#include <QList>

// Convert a uint16_t value to a byte array, little-endian
QByteArray uint16ToByteArrayLittle(uint16_t value);

// Convert a uint16_t value to a byte array, big-endian
QByteArray uint16ToByteArrayBig(int value);

// Convert a uint32_t value to a byte array, big-endian
QByteArray uint32ToByteArray(uint32_t value);

// Convert a byte array to a uint32_t value, big-endian
uint32_t byteArrayToUint32(const QByteArray& data);

// Split a qbytearray into regularly-sized pieces
QList<QByteArray> chunkData(const QByteArray &data, unsigned int chunkSize);

// Pad a byte array to a multile of the given boundary size
void padToBoundary(QByteArray &data, unsigned int boundary);


#endif // BYTEARRAYHELPERS_H
