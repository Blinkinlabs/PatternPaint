#include "lightbuddycommands.h"

#include <QDebug>

namespace LightBuddyCommands {
QByteArray commandHeader()
{
    QByteArray output;
    for (int i = 0; i < 12; i++)
        output.append((char)0xFF);
    return output;
}

QByteArray encodeInt(int data)
{
    QByteArray output;
    output.append((char)((data >> 24) & 0xFF));
    output.append((char)((data >> 16) & 0xFF));
    output.append((char)((data >>  8) & 0xFF));
    output.append((char)((data) & 0xFF));
    return output;
}

SerialCommand eraseFlash()
{
    QByteArray command;
    command + commandHeader();

    command.append((char)0x20); // Erase flash
    command.append('E');        // Checksum sequence
    command.append('e');

    QByteArray ret;
    ret.append('P');
    ret.append((char)0x00);
    ret.append((char)0x00);

    QByteArray mask;
    mask.append((char)0xFF);
    mask.append((char)0x00);
    mask.append((char)0x00);

    // Note: only the length matters for the response, the response data
    // will be ignored.
    return SerialCommand("eraseFlash", command, ret, mask);
}

SerialCommand fileNew(int sizeBytes)
{
    QByteArray command;
    command + commandHeader();

    command.append((char)0x18);   // New file
    command.append((char)0x12);   // filetype = animation
    command += encodeInt(sizeBytes);

    QByteArray ret;
    ret.append('P');
    ret.append((char)0x03);
    ret.append((char)0x00);     // 4 bytes containing the sector data
    ret.append((char)0x00);
    ret.append((char)0x00);
    ret.append((char)0x00);

    QByteArray mask;
    mask.append((char)0xFF);
    mask.append((char)0xFF);
    mask.append((char)0x00);
    mask.append((char)0x00);
    mask.append((char)0x00);
    mask.append((char)0x00);

    return SerialCommand("fileNew", command, ret, mask);
}

SerialCommand writePage(int sector, int offset, QByteArray data)
{
    if (data.size() != 256) {
        // TODO: How to error out here?
        qCritical() << "Data size not multiple of page size!";
    }

    QByteArray command;
    command + commandHeader();

    command.append((char)0x19);     // Write page
    command += encodeInt(sector);
    command += encodeInt(offset);
    command += data;

    QByteArray ret;
    ret.append('P');
    ret.append((char)0x00);
    ret.append((char)0x00);

    QByteArray mask;
    mask.append((char)0xFF);
    mask.append((char)0xFF);
    mask.append((char)0x00);

    // Note: only the length matters for the response, the response data
    // will be ignored.
    return SerialCommand("writePage", command, ret, mask);
}

SerialCommand reloadAnimations()
{
    QByteArray command;
    command + commandHeader();

    command.append((char)0x02);     // Reload animations

    QByteArray ret;
    ret.append('P');
    ret.append((char)0x00);
    ret.append((char)0x00);

    QByteArray mask;
    mask.append((char)0xFF);
    mask.append((char)0xFF);
    mask.append((char)0x00);

    // Note: only the length matters for the response, the response data
    // will be ignored.
    return SerialCommand("reloadAnimations", command, ret, mask);
}
}
