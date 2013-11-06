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


SOURCES += main.cpp\
        mainwindow.cpp \
    blinkytape.cpp \
    patterneditor.cpp \
    colorpicker.cpp \
    ledwriter.cpp \
    avrprogrammer.cpp

HEADERS  += mainwindow.h \
    blinkytape.h \
    patterneditor.h \
    colorpicker.h \
    ledwriter.h \
    avrprogrammer.h \
    PatternPlayer_Sketch.h

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc
