#ifndef BYTEARRAYHELPERS_H
#define BYTEARRAYHELPERS_H

#include "libblinkyglobal.h"

#include <QByteArray>
#include <QList>

namespace ByteArrayHelpers {

// Convert a uint16_t value to a byte array, little-endian
LIBBLINKY_EXPORT QByteArray uint16ToByteArrayLittle(uint16_t value);

// Convert a uint16_t value to a byte array, big-endian
LIBBLINKY_EXPORT QByteArray uint16ToByteArrayBig(int value);

// Convert a uint32_t value to a byte array, little-endian
LIBBLINKY_EXPORT QByteArray uint32ToByteArrayLittle(uint32_t value);

// Convert a uint32_t value to a byte array, big-endian
LIBBLINKY_EXPORT QByteArray uint32ToByteArrayBig(uint32_t value);

// Convert a byte array to a uint32_t value, big-endian
LIBBLINKY_EXPORT uint32_t byteArrayToUint32Little(const QByteArray& data);

// Convert a byte array to a uint32_t value, big-endian
LIBBLINKY_EXPORT uint32_t byteArrayToUint32Big(const QByteArray& data);

// Split a qbytearray into regularly-sized pieces
LIBBLINKY_EXPORT QList<QByteArray> chunkData(const QByteArray &data, unsigned int chunkSize);

// Pad a byte array to a multile of the given boundary size
LIBBLINKY_EXPORT void padToBoundary(QByteArray &data, unsigned int boundary);

}

#endif // BYTEARRAYHELPERS_H
