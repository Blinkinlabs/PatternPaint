#include "bytearrayhelpers.h"

QByteArray uint16ToByteArrayLittle(uint16_t value)
{
    QByteArray data;

    data.append((value >> 0) & 0xFF);
    data.append((value >> 8) & 0xFF);

    return data;
}

QByteArray uint16ToByteArrayBig(int value)
{
    QByteArray data;

    // If the value is out of bounds, return a zero-length byte array
    if((value < 0) || (value > std::numeric_limits<uint16_t>::max()))
        return data;

    data.append((value >> 8) & 0xFF);
    data.append((value >> 0) & 0xFF);

    return data;
}

QByteArray uint32ToByteArray(uint32_t value)
{
    QByteArray data;

    data.append((value >> 24) & 0xFF);
    data.append((value >> 16) & 0xFF);
    data.append((value >> 8) & 0xFF);
    data.append((value >> 0) & 0xFF);

    return data;
}

QList<QByteArray> chunkData(const QByteArray &data, unsigned int chunkSize)
{
    QList<QByteArray> chunks;

    if(chunkSize == 0)
        return chunks;

    for (int position = 0; position < data.length(); position += chunkSize)
        // Note: if chunkSize is larger than the data available,
        // mid() only returns available data.
        chunks.append(data.mid(position, chunkSize));

    return chunks;
}

void padToBoundary(QByteArray &data, unsigned int boundary)
{
    if(boundary == 0)
        return;

    if(boundary > std::numeric_limits<int>::max())
        return;

    if (data.size() % boundary != 0)
        data.append(boundary - (data.size()%boundary), 0xFF);
}
