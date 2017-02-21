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

include(../libusb.pri)
include(../libblinky.pri)

HEADERS += \
    avr109commandstests.h \
    colormodetests.h \
    serialcommandtests.h \
    bytearrayhelperstests.h \
    memorysectiontests.h \
    memorymaptests.h \
    exponentialbrightnesstests.h \
    blinkypendantcommandstests.h \
    lightbuddycommandstests.h \
    fixturetests.h \
    matrixfixturetests.h \
    linearfixturetests.h

SOURCES += \
    avr109commandstests.cpp \
    main.cpp \
    colormodetests.cpp \
    serialcommandtests.cpp \
    bytearrayhelperstests.cpp \
    memorysectiontests.cpp \
    memorymaptests.cpp \
    exponentialbrightnesstests.cpp \
    blinkypendantcommandstests.cpp \
    lightbuddycommandstests.cpp \
    fixturetests.cpp \
    matrixfixturetests.cpp \
    linearfixturetests.cpp
