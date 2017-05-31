QT += core widgets

TARGET = blinky
TEMPLATE = lib

include(../libusb.pri)

HEADERS += \
    fixture.h \
    matrixfixture.h \
    colormode.h \
    pattern.h \
    patternmodel.h \
    patternwriter.h \
    patternframemodel.h \
    patternframeundocommand.h \
    patternscrollmodel.h \
    patternscrollundocommand.h \
    patterncollection.h \
    patterncollectionmodel.h \
    blinkycontroller.h \
    controllerinfo.h \
    blinkyuploader.h \
    blinkytapecontrollerinfo.h \
    avr109commands.h \
    serialcommand.h \
    lightbuddycommands.h \
    blinkypendantcommands.h \
    bytearrayhelpers.h \
    blinkytapeuploaddata.h \
    memorysection.h \
    scenetemplate.h \
    exponentialbrightness.h \
    brightnessmodel.h \
    linearfixture.h \
    firmwarestore.h \
    firmwarereader.h \
    memorymap.h \
    blinkypendantuploaddata.h \
    eightbyeightcommands.h \
    eightbyeightuploader.h \
    esp8266bootloadercommands.h \
    esp8266firmwareloader.h

SOURCES +=  \
    fixture.cpp \
    matrixfixture.cpp \
    colormode.cpp \
    pattern.cpp \
    patternwriter.cpp \
    patternframemodel.cpp \
    patternframeundocommand.cpp \
    patternscrollmodel.cpp \
    patternscrollundocommand.cpp \
    patterncollection.cpp \
    patterncollectionmodel.cpp \
    blinkycontroller.cpp \
    controllerinfo.cpp \
    blinkytapecontrollerinfo.cpp \
    avr109commands.cpp \
    serialcommand.cpp \
    lightbuddycommands.cpp \
    blinkypendantcommands.cpp \
    bytearrayhelpers.cpp \
    blinkytapeuploaddata.cpp \
    memorysection.cpp \
    scenetemplate.cpp \
    exponentialbrightness.cpp \
    linearfixture.cpp \
    brightnessmodel.cpp \
    firmwarestore.cpp \
    firmwarereader.cpp \
    memorymap.cpp \
    blinkypendantuploaddata.cpp \
    eightbyeightcommands.cpp \
    eightbyeightuploader.cpp \
    esp8266bootloadercommands.cpp \
    esp8266firmwareloader.cpp

# For platforms that have serial ports
# TODO: Pull hard-coded references to BlinkyTape from the gui, allowing this section to be switchable
#has_serial {
    QT += serialport

    HEADERS += \
        blinkypendantuploader.h \
        blinkytapeuploader.h \
        serialcommandqueue.h \
        blinkytape.h \
        lightbuddyuploader.h \
        usbutils.h

    SOURCES += \
        blinkypendantuploader.cpp \
        blinkytapeuploader.cpp \
        serialcommandqueue.cpp \
        blinkytape.cpp \
        lightbuddyuploader.cpp \
        usbutils.cpp

#}

RESOURCES += \
    firmware/firmware.qrc
