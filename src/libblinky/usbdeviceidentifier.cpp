#include "usbdeviceidentifier.h"

// TODO: Put these in JSON, allow them to be loaded from an external file
#define BLINKYTAPE_SKETCH_VID           (uint16_t)0x1D50
#define BLINKYTAPE_SKETCH_PID           (uint16_t)0x605E
#define BLINKYTAPE_BOOTLOADER_VID       (uint16_t)0x1D50
#define BLINKYTAPE_BOOTLOADER_PID       (uint16_t)0x606C

#define LEONARDO_SKETCH_VID             (uint16_t)0x2341
#define LEONARDO_SKETCH_PID             (uint16_t)0x8036
#define LEONARDO_BOOTLOADER_VID         (uint16_t)0x2341
#define LEONARDO_BOOTLOADER_PID         (uint16_t)0x0036

#define ARDUINOMICRO_SKETCH_VID         (uint16_t)0x2341
#define ARDUINOMICRO_SKETCH_PID         (uint16_t)0x8037
#define ARDUINOMICRO_BOOTLOADER_VID     (uint16_t)0x2341
#define ARDUINOMICRO_BOOTLOADER_PID     (uint16_t)0x0037

#define BLINKYPENDANT_SKETCH_VID        (uint16_t)0x1209
#define BLINKYPENDANT_SKETCH_PID        (uint16_t)0x8888
#define BLINKYPENDANT_BOOTLOADER_VID    (uint16_t)0x1209
#define BLINKYPENDANT_BOOTLOADER_PID    (uint16_t)0x8889

#define LIGHTBUDDY_SKETCH_VID           (uint16_t)0x1d50
#define LIGHTBUDDY_SKETCH_PID           (uint16_t)0x60aa
#define LIGHTBUDDY_BOOTLOADER_VID       (uint16_t)0x1d50
#define LIGHTBUDDY_BOOTLOADER_PID       (uint16_t)0x60a9

#define EIGHTBYEIGHT_SKETCH_VID         (uint16_t)0x1d50
#define EIGHTBYEIGHT_SKETCH_PID         (uint16_t)0x60f6
#define EIGHTBYEIGHT_BOOTLOADER_VID     (uint16_t)0x1d50
#define EIGHTBYEIGHT_BOOTLOADER_PID     (uint16_t)0x60f5

UsbDeviceIdentifier blinkyControllers[] = {
    {BLINKYTAPE_SKETCH_VID, BLINKYTAPE_SKETCH_PID, "BlinkyTape"},
    {LEONARDO_SKETCH_VID, LEONARDO_SKETCH_PID, "BlinkyTape"},
    {ARDUINOMICRO_SKETCH_VID, ARDUINOMICRO_SKETCH_PID, "BlinkyTape"},
    {BLINKYPENDANT_SKETCH_VID, BLINKYPENDANT_SKETCH_PID, "BlinkyTape"},
    {LIGHTBUDDY_SKETCH_VID, LIGHTBUDDY_SKETCH_PID, "BlinkyTape"},
    {EIGHTBYEIGHT_SKETCH_VID, EIGHTBYEIGHT_SKETCH_PID, "BlinkyTape"},
};

UsbDeviceIdentifier patternUploaders[] = {
    {BLINKYTAPE_SKETCH_VID, BLINKYTAPE_SKETCH_PID, "BlinkyTapeUploader"},
    {LEONARDO_SKETCH_VID, LEONARDO_SKETCH_PID, "BlinkyTapeUploader"},
    {ARDUINOMICRO_SKETCH_VID, ARDUINOMICRO_SKETCH_PID, "BlinkyTapeUploader"},
    {BLINKYPENDANT_SKETCH_VID, BLINKYPENDANT_SKETCH_PID, "BlinkyPendantUploader"},
    {LIGHTBUDDY_SKETCH_VID, LIGHTBUDDY_SKETCH_PID, "LightBuddyUploader"},
    {EIGHTBYEIGHT_SKETCH_VID, EIGHTBYEIGHT_SKETCH_PID, "EightByEightUploader"},
};

UsbDeviceIdentifier firmwareLoaders[] = {
    // TODO: make some intermediate loader here, that resets the Caterina-style device
    {BLINKYTAPE_SKETCH_VID, BLINKYTAPE_SKETCH_PID, "Avr109FirmwareLoader"},
    {LEONARDO_SKETCH_VID, LEONARDO_SKETCH_PID, "Avr109FirmwareLoader"},

    {BLINKYTAPE_BOOTLOADER_VID, BLINKYTAPE_BOOTLOADER_PID, "Avr109FirmwareLoader"},
    {LEONARDO_BOOTLOADER_VID, LEONARDO_BOOTLOADER_PID, "Avr109FirmwareLoader"},
    {ARDUINOMICRO_BOOTLOADER_VID, ARDUINOMICRO_BOOTLOADER_PID, "Avr109FirmwareLoader"},
    {EIGHTBYEIGHT_SKETCH_VID, EIGHTBYEIGHT_SKETCH_PID, "Esp8266FirmwareLoader"},
};

bool UsbDeviceIdentifier::matches(const QSerialPortInfo &info) const
{
    if(info.vendorIdentifier() != vid)
        return false;

    if(info.productIdentifier() != pid)
        return false;

    return true;
}
