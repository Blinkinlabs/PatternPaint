# myclock/myclock.pro
TEMPLATE = subdirs
SUBDIRS = libblinky app

app.depends = libblinky


DISTFILES += \
    gitversion.pri
