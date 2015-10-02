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


    LIBUSB_PATH = $$PWD/../../thirdparty/libusb-1.0.20/libusb

    #INCLUDEPATH += $$PWD/../../thirdparty/libusb-1.0.20/libusb
    INCLUDEPATH += $$LIBUSB_PATH
    QMAKE_LFLAGS += -L $$LIBUSB_PATH/.libs
    LIBS += -lusb-1.0

    # Copy libusb into the app bundle
    libusb.files = $$LIBUSB_PATH/.libs/libusb-1.0.0.dylib
    libusb.path = Contents/Frameworks
    QMAKE_BUNDLE_DATA += libusb

    # And add to the rpath so that the app can find the library
    QMAKE_RPATHDIR += @executable_path/../Frameworks
}
