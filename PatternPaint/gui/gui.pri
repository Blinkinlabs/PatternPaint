INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/mainwindow.h \
    $$PWD/systeminformation.h \
    $$PWD/aboutpatternpaint.h \
    $$PWD/letterboxscrollarea.h \
    $$PWD/colorchooser.h \
    $$PWD/patternframemodel.h \
    $$PWD/pattern.h \
    $$PWD/patterncollectionmodel.h \
    $$PWD/patterncollectiondelegate.h \
    $$PWD/patterncollection.h \
    $$PWD/frameeditor.h \
    $$PWD/patternwriter.h \
    $$PWD/patternscrollmodel.h \
    $$PWD/patternmodel.h \
    $$PWD/patterndelegate.h \
    $$PWD/patternframeundocommand.h \
    $$PWD/patternscrollundocommand.h \
    $$PWD/welcomescreen.h \
    $$PWD/preferences.h \
    $$PWD/sceneconfiguration.h \
    $$PWD/defaults.h \
    $$PWD/scenetemplate.h \
    $$PWD/patternframelistview.h \
    $$PWD/eventratelimiter.h \
    $$PWD/outputpreview.h


SOURCES += \
    $$PWD/mainwindow.cpp \
    $$PWD/systeminformation.cpp \
    $$PWD/aboutpatternpaint.cpp \
    $$PWD/letterboxscrollarea.cpp \
    $$PWD/colorchooser.cpp \
    $$PWD/patternframemodel.cpp \
    $$PWD/pattern.cpp \
    $$PWD/patterncollectionmodel.cpp \
    $$PWD/patterncollectiondelegate.cpp \
    $$PWD/patterncollection.cpp \
    $$PWD/frameeditor.cpp \
    $$PWD/patternwriter.cpp \
    $$PWD/patternscrollmodel.cpp \
    $$PWD/patterndelegate.cpp \
    $$PWD/patternframeundocommand.cpp \
    $$PWD/patternscrollundocommand.cpp \
    $$PWD/welcomescreen.cpp \
    $$PWD/preferences.cpp \
    $$PWD/sceneconfiguration.cpp \
    $$PWD/scenetemplate.cpp \
    $$PWD/patternframelistview.cpp \
    $$PWD/eventratelimiter.cpp \
    $$PWD/outputpreview.cpp

FORMS += \
    $$PWD/mainwindow.ui \
    $$PWD/systeminformation.ui \
    $$PWD/aboutpatternpaint.ui \
    $$PWD/welcomescreen.ui \
    $$PWD/preferences.ui \
    $$PWD/sceneconfiguration.ui

macx {
    # App nap, to prevent macOS from sleeping PatternPaint
    HEADERS  += \
        $$PWD/appnap.h

    OBJECTIVE_SOURCES += \
        $$PWD/appnap.mm
}
