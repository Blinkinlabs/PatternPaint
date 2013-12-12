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
VERSION_MAJOR = 0
VERSION_MINOR = 2
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
    animationuploader.cpp \
    systeminformation.cpp \
    animation.cpp \
    animationeditor.cpp \
    colormodel.cpp \
    aboutpatternpaint.cpp \
    resizeanimation.cpp

HEADERS  += mainwindow.h \
    blinkytape.h \
    colorpicker.h \
    avrprogrammer.h \
    animationuploader.h \
    PatternPlayer_Sketch.h \
    systeminformation.h \
    animation.h \
    animationeditor.h \
    ColorSwirl_Sketch.h \
    colormodel.h \
    aboutpatternpaint.h \
    resizeanimation.h

FORMS    += mainwindow.ui \
    systeminformation.ui \
    aboutpatternpaint.ui \
    resizeanimation.ui

# OS X icon (TODO: Try removing to make sure it has an effect)
ICON = images/blinkytape.icns

# Windows icon
RC_FILE += app.rc

RESOURCES += \
    images.qrc

OTHER_FILES += \
    app.rc
