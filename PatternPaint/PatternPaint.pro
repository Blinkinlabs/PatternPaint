#-------------------------------------------------
#
# Project created by QtCreator 2013-09-30T22:28:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += widgets serialport
} else {
    include($$QTSERIALPORT_PROJECT_ROOT/src/serialport/qt4support/serialport.prf)
}

OBJECTS_DIR = tmp
MOC_DIR = $$OBJECTS_DIR/moc
RCC_DIR = $$OBJECTS_DIR/rcc
UI_DIR = $$OBJECTS_DIR/uic

TARGET = PatternPaint
TEMPLATE = app

#Application version
VERSION_MAJOR = 1
VERSION_MINOR = 7
VERSION_BUILD = 2

#Target version
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}

DEFINES += VERSION_STRING=\\\"$${VERSION}\\\"
DEFINES += APPLICATION_NAME=\\\"$${TARGET}\\\"
DEFINES += ORGANIZATION_NAME=\\\"Blinkinlabs\\\"
DEFINES += ORGANIZATION_DOMAIN=\\\"blinkinlabs.com\\\"

mac {
    QMAKE_TARGET_BUNDLE_PREFIX = \\\"com.blinkinlabs.$${TARGET}\\\"
}

SOURCES += main.cpp\
    mainwindow.cpp \
    colorpicker.cpp \
    systeminformation.cpp \
    colormodel.cpp \
    aboutpatternpaint.cpp \
    pattern.cpp \
    patterneditor.cpp \
    resizepattern.cpp \
    addressprogrammer.cpp \
    letterboxscrollarea.cpp \
    undocommand.cpp \
    colorchooser.cpp \
    patternitemdelegate.cpp \
    patternitem.cpp

HEADERS  += mainwindow.h \
    colorpicker.h \
    systeminformation.h \
    colormodel.h \
    aboutpatternpaint.h \
    pattern.h \
    patterneditor.h \
    resizepattern.h \
    addressprogrammer.h \
    letterboxscrollarea.h \
    undocommand.h \
    colorchooser.h \
    patternitemdelegate.h \
    patternitem.h

mac {
HEADERS  += \
    appnap.h
OBJECTIVE_SOURCES += \
    appnap.mm
}


FORMS    += mainwindow.ui \
    systeminformation.ui \
    aboutpatternpaint.ui \
    resizepattern.ui \
    addressprogrammer.ui

include(instruments/instruments.pri)
include(devices/devices.pri)
include(updater/updater.pri)

mac {
    # OS X: Specify icon resource to use
    ICON = images/patternpaint.icns

    # OS X: Disable app nap using custom plist file
    QMAKE_INFO_PLIST = Info.plist
}

win32 {
    # Windows: Specify icon in custom rc file
    RC_FILE += app.rc
}

RESOURCES += \
    images.qrc

OTHER_FILES += \
    app.rc \
    README.md


## TODO: Am i needed?
#DISTFILES += \
#    Info.plist

OBJECTIVE_SOURCES += \
    appnap.mm
