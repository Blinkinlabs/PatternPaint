INCLUDEPATH += $$PWD

HEADERS += $$PWD/avrprogrammer.h \
    $$PWD/blinkycontroller.h \
    $$PWD/blinkypendantuploader.h \
    $$PWD/blinkytapeuploader.h \
    $$PWD/serialcommandqueue.h \
    $$PWD/avruploaddata.h \
    $$PWD/blinkypendantprogrammer.h \
    $$PWD/blinkytape.h \
    $$PWD/lightbuddyprogrammer.h \
    $$PWD/lightbuddyuploader.h \
    $$PWD/patternuploader.h \
    $$PWD/PatternPlayer_Sketch.h \
    $$PWD/ProductionSketch.h \
    $$PWD/usbutils.h


SOURCES +=  $$PWD/avrprogrammer.cpp \
    $$PWD/blinkycontroller.cpp \
    $$PWD/blinkypendantuploader.cpp \
    $$PWD/blinkytapeuploader.cpp \
    $$PWD/serialcommandqueue.cpp \
    $$PWD/avruploaddata.cpp \
    $$PWD/blinkypendantprogrammer.cpp \
    $$PWD/blinkytape.cpp \
    $$PWD/lightbuddyprogrammer.cpp \
    $$PWD/lightbuddyuploader.cpp \
    $$PWD/usbutils.cpp


# For libusb on OS X
mac {
    LIBUSB_PATH = $$PWD/../../thirdparty/libusb-1.0.20/osx-install

    INCLUDEPATH += $$LIBUSB_PATH/include
    QMAKE_LFLAGS += -L $$LIBUSB_PATH/lib
    LIBS += -lusb-1.0

    # Copy libusb into the app bundle
    libusb.files = $$LIBUSB_PATH/lib/libusb-1.0.0.dylib
    libusb.path = Contents/Frameworks
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

