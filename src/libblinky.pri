CONFIG(release, debug|release): LIBBLINKYDIR=$$OUT_PWD/../libblinky/release
else:CONFIG(debug, debug|release): LIBBLINKYDIR=$$OUT_PWD/../libblinky/debug

INCLUDEPATH += $$PWD/libblinky
DEPENDPATH += $$PWD/libblinky

LIBS += -L$$LIBBLINKYDIR -lblinky

# TODO: Version strategy?

macx {
    # Copy libusb into the app bundle
    libblinky.path = Contents/Frameworks
    libblinky.files = $$LIBBLINKYDIR/libblinky.1.0.0.dylib
    QMAKE_BUNDLE_DATA += libblinky

    # And add to the rpath so that the app can find the library
    QMAKE_RPATHDIR += @executable_path/../Frameworks
}
