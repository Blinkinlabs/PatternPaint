#-------------------------------------------------
#
# Pattern Paint
# http://blinkinlabs.com/patternpaint
#
# Copyright 2013, 2014, 2015, 2016 Blinkinlabs, LLC
#
#-------------------------------------------------

QT += widgets gui core

TARGET = PatternPaint
TEMPLATE = app

OBJECTS_DIR = .tmp
MOC_DIR = .tmp
RCC_DIR = .tmp
UI_DIR = .tmp

include(../libusb.pri)
include(../libblinky.pri)


# TODO: Push this entirely into libblinky, and don't refer
# serial directly in the GUI
    QT += serialport


#Target version and application information
#VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}
include(../gitversion.pri)


DEFINES += VERSION_STRING=\\\"$${VERSION}\\\"
DEFINES += APPLICATION_NAME=\\\"$${TARGET}\\\"
DEFINES += ORGANIZATION_NAME=\\\"Blinkinlabs\\\"
DEFINES += ORGANIZATION_DOMAIN=\\\"blinkinlabs.com\\\"

mac {
    QMAKE_TARGET_BUNDLE_PREFIX = com.blinkinlabs
}

# Disable updater checks when debugging, since the test app isn't signed and will fail anyway.
#TODO: This doesn't work
CONFIG(debug, debug|release) {
    message("Debug build, disabling app update checks")
    DEFINES += DISABLE_UPDATE_CHECKS
}

# Workaround for lineinstrument tool in QT 5.2 - 5.4
# See: https://github.com/Blinkinlabs/PatternPaint/issues/66
equals(QT_MAJOR_VERSION, 5){
    greaterThan(QT_MINOR_VERSION,1) {
        lessThan(QT_MINOR_VERSION,5) {
            DEFINES += LINE_INSTRUMENT_WORKAROUND
        }
    }
}

# Workaround for issue 106, caused by a bug in QT 5.5.1 and 5.6.0rc: progress bar appears after creation
# see: https://github.com/Blinkinlabs/PatternPaint/issues/106
# Qt issue: https://bugreports.qt.io/browse/QTBUG-47042
equals(QT_MAJOR_VERSION, 5){
    greaterThan(QT_MINOR_VERSION,4) {
        DEFINES += PROGRESS_DIALOG_WORKAROUND
    }
}


RESOURCES += \
    images.qrc \
    examples.qrc

include(gui/gui.pri)
include(instruments/instruments.pri)
include(updater/updater.pri)


macx {
    # OS X: Specify icon resource to use
    ICON = images/patternpaint.icns

    # OS X: Specify our developer information using a custom plist
    QMAKE_INFO_PLIST = Info.plist

}

win32 {
    # Windows: Specify the icon to use
    RC_ICONS += images/patternpaint.ico
}
