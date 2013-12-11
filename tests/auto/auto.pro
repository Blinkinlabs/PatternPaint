#-------------------------------------------------
#
# Project created by QtCreator 2013-12-11T17:18:47
#
#-------------------------------------------------

QT       += core gui testlib

TARGET = tst_autotest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    tst_animation.cpp

# Include the PatternPaint project
INCLUDEPATH += ../../src .

SOURCES += ../../src/PatternPaint/animation.cpp \
            ../../src/PatternPaint/colormodel.cpp \
