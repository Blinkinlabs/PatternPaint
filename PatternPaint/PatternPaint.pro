#-------------------------------------------------
#
# Project created by QtCreator 2013-09-30T22:28:07
#
#-------------------------------------------------

QT       += core gui
QT       += widgets serialport

OBJECTS_DIR = tmp
MOC_DIR = $$OBJECTS_DIR/moc
RCC_DIR = $$OBJECTS_DIR/rcc
UI_DIR = $$OBJECTS_DIR/uic

TARGET = PatternPaint
TEMPLATE = app

#Application version
VERSION_MAJOR = 2
VERSION_MINOR = 0
VERSION_BUILD = 0

#Target version and application information
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}

DEFINES += VERSION_STRING=\\\"$${VERSION}\\\"
DEFINES += APPLICATION_NAME=\\\"$${TARGET}\\\"
DEFINES += ORGANIZATION_NAME=\\\"Blinkinlabs\\\"
DEFINES += ORGANIZATION_DOMAIN=\\\"blinkinlabs.com\\\"

mac {
    QMAKE_TARGET_BUNDLE_PREFIX = com.blinkinlabs
}

# Disable updater checks when debugging, since the test app isn't signed and will fail anyway.
CONFIG(debug, debug|release) {
    DEFINES += DISABLE_UPDATE_CHECKS
}

# Workaround for lineinstrument tool in QT 5.2 - 5.4
# See: https://github.com/Blinkinlabs/PatternPaint/issues/66
# TODO: Test+remove me when updating to QT 5.5
equals(QT_MAJOR_VERSION, 5){
    greaterThan(QT_MINOR_VERSION,1) {
        lessThan(QT_MINOR_VERSION,5) {
            DEFINES += LINE_INSTRUMENT_WORKAROUND
        }
    }
}

SOURCES += main.cpp\
    mainwindow.cpp \
    systeminformation.cpp \
    aboutpatternpaint.cpp \
    addressprogrammer.cpp \
    letterboxscrollarea.cpp \
    colorchooser.cpp \
    patternframemodel.cpp \
    pattern.cpp \
    patterncollectionmodel.cpp \
    patterncollectionlistview.cpp \
    patterncollectiondelegate.cpp \
    patterncollection.cpp \
    frameeditor.cpp \
    patternwriter.cpp \
    patternscrollmodel.cpp \
    patterndelegate.cpp \
    patternframeundocommand.cpp \
    patternscrollundocommand.cpp \
    fixturesettings.cpp

HEADERS  += mainwindow.h \
    systeminformation.h \
    aboutpatternpaint.h \
    addressprogrammer.h \
    letterboxscrollarea.h \
    colorchooser.h \
    patternframemodel.h \
    pattern.h \
    patterncollectionmodel.h \
    patterncollectionlistview.h \
    patterncollectiondelegate.h \
    patterncollection.h \
    frameeditor.h \
    patternwriter.h \
    patternscrollmodel.h \
    patternmodel.h \
    patterndelegate.h \
    colors.h \
    patternframeundocommand.h \
    patternscrollundocommand.h \
    fixturesettings.h

mac {
HEADERS  += \
    appnap.h
OBJECTIVE_SOURCES += \
    appnap.mm
}


FORMS    += mainwindow.ui \
    systeminformation.ui \
    aboutpatternpaint.ui \
    addressprogrammer.ui \
    fixturesettings.ui

include(instruments/instruments.pri)
include(controllers/controllers.pri)
include(fixtures/fixtures.pri)
include(updater/updater.pri)

mac {
    # OS X: Specify icon resource to use
    ICON = images/patternpaint.icns

    # OS X: Specify our developer information using a custom plist
    QMAKE_INFO_PLIST = Info.plist

    OBJECTIVE_SOURCES += \
        appnap.mm
}

win32 {
    # Windows: Specify the icon to use
    RC_ICONS += images/patternpaint.ico
}

RESOURCES += \
    images.qrc \
    examples.qrc

OTHER_FILES += \
    README.md

