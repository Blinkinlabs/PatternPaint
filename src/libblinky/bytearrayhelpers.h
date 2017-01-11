#ifndef BYTEARRAYHELPERS_H
#define BYTEARRAYHELPERS_H

#include <QByteArray>
#include <QList>

// Utility function to transmit a uint16_t value
QByteArray uint16ToByteArray(int value);

// Utility function to split a qbytearray into regularly-sized pieces
QList<QByteArray> chunkData(const QByteArray &data, int chunkSize);


#endif // BYTEARRAYHELPERS_H
