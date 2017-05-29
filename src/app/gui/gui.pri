INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/mainwindow.h \
    $$PWD/systeminformation.h \
    $$PWD/aboutpatternpaint.h \
    $$PWD/colorchooser.h \
    $$PWD/patterncollectiondelegate.h \
    $$PWD/frameeditor.h \
    $$PWD/patterndelegate.h \
    $$PWD/welcomescreen.h \
    $$PWD/preferences.h \
    $$PWD/sceneconfiguration.h \
    $$PWD/defaults.h \
    $$PWD/patternframelistview.h \
    $$PWD/outputpreview.h \
    $$PWD/debuglog.h \
    $$PWD/firmwaremanager.h \
    $$PWD/intervalfilter.h \
    $$PWD/instrumentconfiguration.h


SOURCES += \
    $$PWD/mainwindow.cpp \
    $$PWD/systeminformation.cpp \
    $$PWD/aboutpatternpaint.cpp \
    $$PWD/colorchooser.cpp \
    $$PWD/patterncollectiondelegate.cpp \
    $$PWD/frameeditor.cpp \
    $$PWD/patterndelegate.cpp \
    $$PWD/welcomescreen.cpp \
    $$PWD/preferences.cpp \
    $$PWD/sceneconfiguration.cpp \
    $$PWD/patternframelistview.cpp \
    $$PWD/outputpreview.cpp \
    $$PWD/main.cpp \
    $$PWD/debuglog.cpp \
    $$PWD/firmwaremanager.cpp \
    $$PWD/intervalfilter.cpp \
    $$PWD/instrumentconfiguration.cpp

FORMS += \
    $$PWD/mainwindow.ui \
    $$PWD/systeminformation.ui \
    $$PWD/aboutpatternpaint.ui \
    $$PWD/welcomescreen.ui \
    $$PWD/preferences.ui \
    $$PWD/sceneconfiguration.ui \
    $$PWD/debuglog.ui \
    $$PWD/firmwaremanager.ui

TRANSLATIONS += \
    $$PWD/language/patternpaint_de.ts

macx {
    # App nap, to prevent macOS from sleeping PatternPaint
    HEADERS  += \
        $$PWD/appnap.h

    OBJECTIVE_SOURCES += \
        $$PWD/appnap.mm
}

RESOURCES += \
    $$PWD/language/language.qrc
