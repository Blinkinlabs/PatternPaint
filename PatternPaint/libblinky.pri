# TODO: How to coerce qmake to do this automatically?
INCLUDEPATH += ../libblinky

CONFIG(debug, debug|release) {
    LIBBLINKYDIR = ../libblinky/debug/
}
else {
    LIBBLINKYDIR = ../libblinky/release/
}
LIBS += -L$$LIBBLINKYDIR -llibblinky
