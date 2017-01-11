#include "bytearrayhelpers.h"

// Utility function to transmit a uint16_t value
QByteArray uint16ToByteArray(int value)
{
    QByteArray data;

    // If the value is out of bounds, return a zero-length byte array
    if((value < 0) || (value > 65535)) {
        return data;
    }

    data.append((value >> 8) & 0xFF);
    data.append((value >> 0) & 0xFF);

    return data;
}

//
QList<QByteArray> chunkData(const QByteArray &data, int chunkSize)
{
    QList<QByteArray> chunks;

    // If the chunk size is invalid, return an empty set
    if(chunkSize <=  0) {
        return chunks;
    }

    for (int position = 0;
         position < data.length();
         position += chunkSize) {
        // Note: if chunkSize is larger than the data available,
        // mid() only returns available data.
        chunks.append(data.mid(position, chunkSize));
    }

    return chunks;
}
