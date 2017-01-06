#-------------------------------------------------
#
# libblinky tests
#
# Copyright 2017 Blinkinlabs, LLC
#
#-------------------------------------------------

QT += core widgets testlib

TARGET = libblinky-test
TEMPLATE = app

OBJECTS_DIR = .tmp
MOC_DIR = .tmp
RCC_DIR = .tmp
UI_DIR = .tmp

include(../libusb.pri)
include(../libblinky.pri)

HEADERS += \
    avr109commandstests.h

SOURCES += \
    avr109commandstests.cpp \
    main.cpp
