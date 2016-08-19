#-------------------------------------------------
#
# Pattern Paint
# http://blinkinlabs.com/patternpaint
#
# Copyright 2013, 2014, 2015, 2016 Blinkinlabs, LLC
#
#-------------------------------------------------

QT += widgets gui core multimedia

# Specificy where to place the object files so they don't clutter the source tree
OBJECTS_DIR = tmp
MOC_DIR = $$OBJECTS_DIR/moc
RCC_DIR = $$OBJECTS_DIR/rcc
UI_DIR = $$OBJECTS_DIR/uic

TARGET = PatternPaint
TEMPLATE = app

#Target version and application information
#VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}

include(gitversion.pri)

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

SOURCES += \
    main.cpp\
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
    welcomescreen.cpp \
    preferences.cpp \
    sceneconfiguration.cpp \
    scenetemplate.cpp \
    patternframelistview.cpp \
    eventratelimiter.cpp \
    outputpreview.cpp

HEADERS  += \
    mainwindow.h \
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
    patternframeundocommand.h \
    patternscrollundocommand.h \
    welcomescreen.h \
    preferences.h \
    sceneconfiguration.h \
    defaults.h \
    scenetemplate.h \
    patternframelistview.h \
    eventratelimiter.h \
    outputpreview.h


FORMS    += \
    mainwindow.ui \
    systeminformation.ui \
    aboutpatternpaint.ui \
    addressprogrammer.ui \
    welcomescreen.ui \
    preferences.ui \
    sceneconfiguration.ui

RESOURCES += \
    images.qrc \
    examples.qrc

include(instruments/instruments.pri)
include(controllers/controllers.pri)
include(fixtures/fixtures.pri)
include(updater/updater.pri)


# Copies the given files to the destination directory
# Using QMAKE_BUNDLE_DATA would be better here, but it fails because it tries
# to interpret the library links as files instead of directories.
defineTest(copySystemFrameworks) {
    files = $$1
    SOURCE_DIR = ~/qt/5.5/clang_64/lib
    DDIR = PatternPaint.app/Contents/Frameworks

    QMAKE_POST_LINK += mkdir -p $$quote($$DDIR) $$escape_expand(\\n\\t)

    for(FILE, files) {
        # If the libraries were already there, remove them.
        QMAKE_POST_LINK += if [ -d $$quote($$DDIR/$$FILE) ] ; then rm -R $$quote($$DDIR/$$FILE); fi $$escape_expand(\\n\\t)

        # Copy the library directory, recursively
        QMAKE_POST_LINK += cp -R $$quote($$SOURCE_DIR/$$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)

        # and remove the .prl files since they will cause the signing process to fail
        QMAKE_POST_LINK += rm $$quote($$DDIR/$$FILE/*.prl) $$escape_expand(\\n\\t)
        QMAKE_POST_LINK += rm $$quote($$DDIR/$$FILE/*_debug) $$escape_expand(\\n\\t)
        QMAKE_POST_LINK += rm $$quote($$DDIR/$$FILE/Versions/5/*_debug) $$escape_expand(\\n\\t)
        QMAKE_POST_LINK += rm $$quote($$DDIR/$$FILE/Headers) $$escape_expand(\\n\\t)
        QMAKE_POST_LINK += rm -R $$quote($$DDIR/$$FILE/Versions/5/Headers/) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

macx {
    # OS X: Specify icon resource to use
    ICON = images/patternpaint.icns

    # OS X: Specify our developer information using a custom plist
    QMAKE_INFO_PLIST = Info.plist

    HEADERS  += \
        appnap.h

    OBJECTIVE_SOURCES += \
        appnap.mm

    # Workaround for broken macdeployqt on Qt 5.5.1: Copy in the system
    # libraries manually. Unfortunately this is pretty broken.
    equals(QT_VERSION, 5.5.1){
        SYSTEM_LIBS += \
            QtCore.framework \
            QtDBus.framework \
            QtGui.framework \
            QtPrintSupport.framework \
            QtSerialPort.framework \
            QtWidgets.framework

        copySystemFrameworks($$SYSTEM_LIBS)

        # And add frameworks to the rpath so that the app can find the framework.
        QMAKE_RPATHDIR += @executable_path/../Frameworks
    }
}

win32 {
    # Windows: Specify the icon to use
    RC_ICONS += images/patternpaint.ico
}

DISTFILES += \
    ../uncrustify.cfg \
    gitversion.pri
