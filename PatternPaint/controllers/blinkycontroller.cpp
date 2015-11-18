#include "blinkycontroller.h"

#include <QList>
#include <QSerialPortInfo>


BlinkyController::BlinkyController(QObject *parent) : QObject(parent)
{

}

void BlinkyController::close()
{
}


// TODO: Support a method for loading these from preferences file
QList<QSerialPortInfo> BlinkyController::probe()
{
    QList<QSerialPortInfo> serialPorts = QSerialPortInfo::availablePorts();
    QList<QSerialPortInfo> tapes;

    foreach (const QSerialPortInfo &info, serialPorts) {
        // Only connect to known BlinkyTapes
        if(info.vendorIdentifier() == BLINKYTAPE_SKETCH_VID
           && info.productIdentifier() == BLINKYTAPE_SKETCH_PID) {
            tapes.push_back(info);
        }
        // If it's a leonardo, it /may/ be a BlinkyTape running a user sketch
        else if(info.vendorIdentifier() == LEONARDO_SKETCH_VID
                && info.productIdentifier() == LEONARDO_SKETCH_PID) {
                 tapes.push_back(info);
        }
        // Also BlinkyPendants!
        else if(info.vendorIdentifier() == BLINKYPENDANT_SKETCH_VID
                && info.productIdentifier() == BLINKYPENDANT_SKETCH_PID) {
                 tapes.push_back(info);
        }
        // And Lightbuddies!
        else if(info.vendorIdentifier() == LIGHTBUDDY_SKETCH_VID
                && info.productIdentifier() == LIGHTBUDDY_SKETCH_PID) {
                 tapes.push_back(info);
        }
    }

    return tapes;
}

// TODO: Support a method for loading these from preferences file
QList<QSerialPortInfo> BlinkyController::probeBootloaders()
{
    QList<QSerialPortInfo> serialPorts = QSerialPortInfo::availablePorts();
    QList<QSerialPortInfo> tapes;

    foreach (const QSerialPortInfo &info, serialPorts) {
        // Only connect to known BlinkyTapes
        if(info.vendorIdentifier() == BLINKYTAPE_BOOTLOADER_VID
           && info.productIdentifier() == BLINKYTAPE_BOOTLOADER_PID) {
            tapes.push_back(info);
        }
        // If it's a leonardo, it /may/ be a BlinkyTape running a user sketch
        else if(info.vendorIdentifier() == LEONARDO_BOOTLOADER_VID
                && info.productIdentifier() == LEONARDO_BOOTLOADER_PID) {
                 tapes.push_back(info);
        }
    }

    return tapes;
}
