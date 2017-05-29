#include "lightbuddycommands.h"

#include <QDebug>

#include "bytearrayhelpers.h"

#define COMMAND_RELOAD_ANIMATIONS 0x02
#define COMMAND_NEW_FILE    0x18
#define COMMAND_WRITE_PAGE  0x19
#define COMMAND_ERASE_FLASH 0x20

#define FLASH_PAGE_SIZE_BYTES 256 // This could be polled using the 'b' command

namespace LightBuddyCommands {

QByteArray commandHeader()
{
    return QByteArray(12, (char)0xFF);
}

SerialCommand eraseFlash()
{
    QByteArray command;
    command.append(commandHeader());
    command.append((char)COMMAND_ERASE_FLASH); // Erase flash
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

    return SerialCommand("eraseFlash", command, ret, mask, 20000);
}

SerialCommand fileNew(uint32_t sizeBytes)
{
    QByteArray command;
    command.append(commandHeader());
    command.append((char)COMMAND_NEW_FILE);   // New file
    command.append((char)0x12);   // filetype = animation
    command.append(ByteArrayCommands::uint32ToByteArrayBig(sizeBytes));

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

    return SerialCommand("fileNew", command, ret, mask, 5000);
}

SerialCommand writePage(int sector, int offset, const QByteArray &data)
{    
    // TODO: How to error out here?
    if(data.length() != FLASH_PAGE_SIZE_BYTES)
        qCritical() << "Bad data size, padding"
                    << "got:" << data.length()
                    << "expected:" << FLASH_PAGE_SIZE_BYTES;

    QByteArray paddedData(data.mid(0,FLASH_PAGE_SIZE_BYTES));

    while (paddedData.length() < FLASH_PAGE_SIZE_BYTES)
        paddedData.append((char)0xFF);


    // TODO: Test that offset is page-aligned

    // TODO: Test that the offset fits into the file? We can't really do that
    // from here, maybe let the firmware handle it.

    QByteArray command;
    command += commandHeader();

    command.append((char)0x19);     // Write page
    command += ByteArrayCommands::uint32ToByteArrayBig(sector);
    command += ByteArrayCommands::uint32ToByteArrayBig(offset);
    command += paddedData;

    QByteArray ret;
    ret.append('P');
    ret.append((char)0x00);
    ret.append((char)0x00);

    QByteArray mask;
    mask.append((char)0xFF);
    mask.append((char)0xFF);
    mask.append((char)0x00);

    return SerialCommand("writePage", command, ret, mask);
}

SerialCommand reloadAnimations()
{
    QByteArray command;
    command += commandHeader();

    command.append((char)COMMAND_RELOAD_ANIMATIONS);     // Reload animations

    QByteArray ret;
    ret.append('P');
    ret.append((char)0x00);
    ret.append((char)0x00);

    QByteArray mask;
    mask.append((char)0xFF);
    mask.append((char)0xFF);
    mask.append((char)0x00);

    return SerialCommand("reloadAnimations", command, ret, mask);
}
}
