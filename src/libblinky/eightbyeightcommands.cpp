#include "eightbyeightcommands.h"
#include "bytearrayhelpers.h"

#include <QDebug>

#define CHUNK_SIZE_BYTES 64 // TODO: Small so we don't overflow the usb/serial converter?

#define MAX_FILENAME_LENGTH 30  // As per the SPIFFS docs

namespace EightByEightCommands {

QByteArray commandHeader()
{
    return QByteArray(12, (char)0xFF);
}

SerialCommand formatFilesystem()
{
    QByteArray command;
    command.append(commandHeader());
    command.append((char)Command_Format_Filesystem); // Erase flash
    command.append((char)0x02); // Data length
    command.append('E');        // Checksum sequence
    command.append('e');

    QByteArray ret;
    ret.append((char)0x00);
    ret.append((char)0x00);

    QByteArray mask;
    mask.append((char)0xFF);
    mask.append((char)0xFF);

    return SerialCommand("formatFilesystem", command, ret, mask, 20000);
}

SerialCommand openFile(const QString fileName, FileMode mode)
{
    // Note: The file system can't support long file names
    QString croppedFileName = fileName.left(MAX_FILENAME_LENGTH);

    QByteArray command;
    command.append(commandHeader());
    command.append((char)Command_Open_File);
    command.append((char)1 + croppedFileName.length() + 1); // Data length
    command.append((char)mode); // File mode
    command.append(croppedFileName.toUtf8());
    command.append((char)0x00);

    QByteArray ret;
    ret.append((char)0x00);
    ret.append((char)0x00);

    QByteArray mask;
    mask.append((char)0xFF);
    mask.append((char)0xFF);

    return SerialCommand("openFile", command, ret, mask);
}

SerialCommand writeChunk(const QByteArray &data)
{
    // TODO: What if the data length is > 255?

    QByteArray command;
    command.append(commandHeader());
    command.append((char)Command_Write);
    command.append((char)data.length()); // Data length
    command.append(data);

    QByteArray ret;
    ret.append((char)0x00);
    ret.append((char)0x01);
    ret.append((char)data.length());

    QByteArray mask;
    mask.append((char)0xFF);
    mask.append((char)0xFF);
    mask.append((char)0xFF);

    return SerialCommand("writeChunk", command, ret, mask);
}

QList<SerialCommand> write(const QByteArray &data)
{
    QByteArray paddedData(data);
    QList<SerialCommand> commands;

    for (QByteArray chunk : ByteArrayCommands::chunkData(paddedData, CHUNK_SIZE_BYTES))
        commands.append(writeChunk(chunk));

    return commands;
}

SerialCommand verifyChunk(const QByteArray &data)
{
    QByteArray command;
    command.append(commandHeader());
    command.append((char)Command_Read);
    command.append((char)1);
    command.append((char)data.length());

    QByteArray ret;
    ret.append((char)0x00);
    ret.append((char)data.length());
    ret.append(data);

    QByteArray mask;
    mask.append((char)0xFF);
    mask.append((char)0xFF);
    mask.append(QByteArray(data.length(),(char)0xFF));

    return SerialCommand("verifyChunk", command, ret, mask);
}

QList<SerialCommand> verify(const QByteArray &data)
{
    QByteArray paddedData(data);
    QList<SerialCommand> commands;

    for (QByteArray chunk : ByteArrayCommands::chunkData(paddedData, CHUNK_SIZE_BYTES))
        commands.append(verifyChunk(chunk));

    return commands;
}

SerialCommand closeFile()
{
    QByteArray command;
    command.append(commandHeader());
    command.append((char)Command_Close_File);
    command.append((char)0x00); // Data length

    QByteArray ret;
    ret.append((char)0x00);
    ret.append((char)0x00);

    QByteArray mask;
    mask.append((char)0xFF);
    mask.append((char)0xFF);

    return SerialCommand("closeFile", command, ret, mask);
}

SerialCommand lockFileAccess()
{
    QByteArray command;
    command.append(commandHeader());
    command.append((char)Command_Lock_File_Access);
    command.append((char)0x00); // Data length

    QByteArray ret;
    ret.append((char)0x00);
    ret.append((char)0x00);

    QByteArray mask;
    mask.append((char)0xFF);
    mask.append((char)0xFF);

    return SerialCommand("lockFileAccess", command, ret, mask);
}

SerialCommand unlockFileAccess()
{
    QByteArray command;
    command.append(commandHeader());
    command.append((char)Command_Unlock_File_Access);
    command.append((char)0x00); // Data length

    QByteArray ret;
    ret.append((char)0x00);
    ret.append((char)0x00);

    QByteArray mask;
    mask.append((char)0xFF);
    mask.append((char)0xFF);

    return SerialCommand("unlockFileAccess", command, ret, mask);
}

SerialCommand getFirmwareVersion()
{
    QByteArray command;
    command.append(commandHeader());
    command.append((char)Command_GetFirmwareVersion);
    command.append((char)0x00); // Data length

    QByteArray ret;
    ret.append((char)0x00);
    ret.append((char)0x04);
    ret.append(QByteArray(4,(char)0x00));

    QByteArray mask;
    mask.append((char)0xFF);
    mask.append((char)0xFF);
    mask.append(QByteArray(4,(char)0x00));  // version is unknown

    return SerialCommand("getFirmwareVersion", command, ret, mask);
}

}
