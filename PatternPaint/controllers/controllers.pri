INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/blinkycontroller.h \
    $$PWD/controllerinfo.h \
    $$PWD/blinkyuploader.h \
    $$PWD/blinkytapecontrollerinfo.h \
    $$PWD/avr109commands.h \
    $$PWD/serialcommand.h \
    $$PWD/lightbuddycommands.h \
    $$PWD/blinkypendantcommands.h

SOURCES += \
    $$PWD/blinkycontroller.cpp \
    $$PWD/controllerinfo.cpp \
    $$PWD/blinkytapecontrollerinfo.cpp \
    $$PWD/avr109commands.cpp \
    $$PWD/serialcommand.cpp \
    $$PWD/lightbuddycommands.cpp \
    $$PWD/blinkypendantcommands.cpp

# For platforms that have serial ports
# TODO: Pull hard-coded references to BlinkyTape from the gui, allowing this section to be switchable
#has_serial {
    QT += serialport

    HEADERS += \
        $$PWD/blinkypendantuploader.h \
        $$PWD/blinkytapeuploader.h \
        $$PWD/serialcommandqueue.h \
        $$PWD/avruploaddata.h \
        $$PWD/blinkytape.h \
        $$PWD/lightbuddyuploader.h \
        $$PWD/PatternPlayer_Sketch.h \
        $$PWD/ProductionSketch.h \
        $$PWD/usbutils.h

    SOURCES += \
        $$PWD/blinkypendantuploader.cpp \
        $$PWD/blinkytapeuploader.cpp \
        $$PWD/serialcommandqueue.cpp \
        $$PWD/avruploaddata.cpp \
        $$PWD/blinkytape.cpp \
        $$PWD/lightbuddyuploader.cpp \
        $$PWD/usbutils.cpp \
#}


# For libusb on OS X
macx {
    LIBUSB_PATH = $$PWD/../../thirdparty/libusb-1.0.20/osx-install

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
    LIBUSB_DLL = $$PWD/../../thirdparty/libusb-1.0.20-win/MinGW32/dll/libusb-1.0.dll

    INCLUDEPATH += $$PWD/../../thirdparty/libusb-1.0.20-win/include/
    LIBS += $$PWD/../../thirdparty/libusb-1.0.20-win/MinGW32/dll/libusb-1.0.dll

    # Copy the Sparkle DLL into the build directory so that it can be used
    QMAKE_PRE_LINK += copy $$shell_path($$LIBUSB_DLL)  $$shell_path($$OUT_PWD) &
}

# For libusb on Linux
linux {
    LIBS += -lusb-1.0
}

