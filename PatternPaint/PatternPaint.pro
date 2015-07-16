#-------------------------------------------------
#
# Project created by QtCreator 2013-09-30T22:28:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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
VERSION_BUILD = 0

DEFINES += "VERSION_MAJOR=$${VERSION_MAJOR}" \
       "VERSION_MINOR=$${VERSION_MINOR}" \
       "VERSION_BUILD=$${VERSION_BUILD}"

#Target version
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}

DEFINES += APPLICATION_NAME=\\\""PatternPaint"\\\"
DEFINES += ORGANIZATION_NAME=\\\""Blinkinlabs"\\\"
DEFINES += ORGANIZATION_DOMAIN=\\\""blinkinlabs.com"\\\"

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
    avrprogrammer.h \
    PatternPlayer_Sketch.h \
    systeminformation.h \
    ColorSwirl_Sketch.h \
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

FORMS    += mainwindow.ui \
    systeminformation.ui \
    aboutpatternpaint.ui \
    resizepattern.ui \
    addressprogrammer.ui

include(instruments/instruments.pri)
include(devices/devices.pri)
include(updater/updater.pri)

# OS X: Specify icon resource to use
ICON = images/patternpaint.icns

# OS X: Disable app nap using custom plist file
QMAKE_INFO_PLIST = Info.plist

# Windows: Specify icon in custom rc file
RC_FILE += app.rc

RESOURCES += \
    images.qrc

OTHER_FILES += \
    app.rc \
    Info.plist \
    README.md
