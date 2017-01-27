# TODO: Why does qmake on windows make release and debug dirs, but on macOS it doesn't?
win32:CONFIG(release, debug|release): LIBBLINKYDIR=$$OUT_PWD/../libblinky/release
else:win32:CONFIG(debug, debug|release): LIBBLINKYDIR=$$OUT_PWD/../libblinky/debug
else:unix: LIBBLINKYDIR=$$OUT_PWD/../libblinky

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


