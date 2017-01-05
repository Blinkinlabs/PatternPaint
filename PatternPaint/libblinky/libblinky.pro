QT += core widgets

TARGET = libblinky
TEMPLATE = lib

OBJECTS_DIR = .tmplib
MOC_DIR = .tmplib
RCC_DIR = .tmplib


HEADERS += \
    fixture.h \
    matrixfixture.h \
    colormode.h \
    brightnessmodel.h\
    pattern.h \
    patternmodel.h \
    patternwriter.h \
    patternframemodel.h \
    patternframeundocommand.h \
    patternscrollmodel.h \
    patternscrollundocommand.h \
    blinkycontroller.h \
    controllerinfo.h \
    blinkyuploader.h \
    blinkytapecontrollerinfo.h \
    avr109commands.h \
    serialcommand.h \
    lightbuddycommands.h \
    blinkypendantcommands.h

SOURCES +=  \
    fixture.cpp \
    matrixfixture.cpp \
    colormode.cpp \
    brightnessmodel.cpp \
    pattern.cpp \
    patternwriter.cpp \
    patternframemodel.cpp \
    patternframeundocommand.cpp \
    patternscrollmodel.cpp \
    patternscrollundocommand.cpp \
    blinkycontroller.cpp \
    controllerinfo.cpp \
    blinkytapecontrollerinfo.cpp \
    avr109commands.cpp \
    serialcommand.cpp \
    lightbuddycommands.cpp \
    blinkypendantcommands.cpp

# For platforms that have serial ports
# TODO: Pull hard-coded references to BlinkyTape from the gui, allowing this section to be switchable
#has_serial {
    QT += serialport

    HEADERS += \
        blinkypendantuploader.h \
        blinkytapeuploader.h \
        serialcommandqueue.h \
        avruploaddata.h \
        blinkytape.h \
        lightbuddyuploader.h \
        PatternPlayer_Sketch.h \
        ProductionSketch.h \
        usbutils.h

    SOURCES += \
        blinkypendantuploader.cpp \
        blinkytapeuploader.cpp \
        serialcommandqueue.cpp \
        avruploaddata.cpp \
        blinkytape.cpp \
        lightbuddyuploader.cpp \
        usbutils.cpp

#}


# For libusb on OS X
macx {
    LIBUSB_PATH = ../../thirdparty/libusb-1.0.20/osx-install

    INCLUDEPATH += $$LIBUSB_PATH/include
    QMAKE_LFLAGS += -L $$LIBUSB_PATH/lib
    LIBS += -lusb-1.0

    # Copy libusb into the app bundle
    libusb.path = Contents/Frameworks
    libusb.files = $$LIBUSB_PATH/lib/libusb-1.0.0.dylib
    QMAKE_BUNDLE_DATA += libusb

    # And add to the rpath so that the app can find the library
    QMAKE_RPATHDIR += @executable_path/../Frameworks
}

# For libusb on Windows
win32 {
    LIBUSB_DLL = ../../thirdparty/libusb-1.0.20-win/MinGW32/dll/libusb-1.0.dll

    INCLUDEPATH += ../../thirdparty/libusb-1.0.20-win/include/
    LIBS += ../../thirdparty/libusb-1.0.20-win/MinGW32/dll/libusb-1.0.dll

    # Copy the Sparkle DLL into the build directory so that it can be used
    QMAKE_PRE_LINK += copy $$shell_path($$LIBUSB_DLL)  $$shell_path($$OUT_PWD) &
}

# For libusb on Linux
linux {
    LIBS += -lusb-1.0
}

