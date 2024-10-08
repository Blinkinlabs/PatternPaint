TEMPLATE = subdirs
SUBDIRS = \
    libblinky \
    libblinky-test \
    app

libblinky-test.depends = libblinky
app.depends = libblinky


DISTFILES += \
    gitversion.pri \
    libusb.pri \
    libblinky.pri
