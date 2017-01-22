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

win32 {
    LIBUSB_DLL = ../../thirdparty/libusb-1.0.20-win/MinGW32/dll/libusb-1.0.dll

    INCLUDEPATH += ../../thirdparty/libusb-1.0.20-win/include/
    LIBS += ../../thirdparty/libusb-1.0.20-win/MinGW32/dll/libusb-1.0.dll

    # Copy the DLL into the build directory so that it can be used
    QMAKE_PRE_LINK += copy $$shell_path($$LIBUSB_DLL)  $$shell_path($$OUT_PWD) &
}

# For libusb on Linux
linux {
    LIBS += -lusb-1.0
}
