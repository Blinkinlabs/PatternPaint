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

TARGET = PatternPaint
TEMPLATE = app

#Application version
VERSION_MAJOR = 1
VERSION_MINOR = 1
VERSION_BUILD = 0

DEFINES += "VERSION_MAJOR=$${VERSION_MAJOR}" \
       "VERSION_MINOR=$${VERSION_MINOR}" \
       "VERSION_BUILD=$${VERSION_BUILD}"

#Target version
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}

SOURCES += main.cpp\
        mainwindow.cpp \
    blinkytape.cpp \
    colorpicker.cpp \
    avrprogrammer.cpp \
    systeminformation.cpp \
    colormodel.cpp \
    aboutpatternpaint.cpp \
    pattern.cpp \
    patterneditor.cpp \
    resizepattern.cpp \
    uploaddata.cpp \
    addressprogrammer.cpp \
    avrpatternuploader.cpp

HEADERS  += mainwindow.h \
    blinkytape.h \
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
    uploaddata.h \
    addressprogrammer.h \
    avrpatternuploader.h \
    patternuploader.h

FORMS    += mainwindow.ui \
    systeminformation.ui \
    aboutpatternpaint.ui \
    resizepattern.ui \
    addressprogrammer.ui

# OS X: Specify icon resource to use
ICON = images/blinkytape.icns

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
