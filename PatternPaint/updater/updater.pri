INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/autoupdater.h

SOURCES += \ 
    $$PWD/autoupdater.cpp

# For sparkle on OS X
mac {
    HEADERS +=  \
        $$PWD/sparkleautoupdater.h \
        $$PWD/cocoainitializer.h
    OBJECTIVE_SOURCES += \
        $$PWD/cocoainitializer.mm \
        $$PWD/sparkleautoupdater.mm
    LIBS += -framework Sparkle -framework AppKit

#    # Find the sparkle library
#    QMAKE_LFLAGS += -F.
#    QMAKE_CXXFLAGS += -F.
#    QMAKE_CFLAGS += -F.
#    QMAKE_OBJECTIVE_CFLAGS += -F.

# TODO: Relative path
    QMAKE_LFLAGS += -F /Users/matt/Blinkinlabs-Projects/PatternPaint/PatternPaint/updater/
    QMAKE_CXXFLAGS += -F /Users/matt/Blinkinlabs-Projects/PatternPaint/PatternPaint/updater/
    QMAKE_CFLAGS += -F /Users/matt/Blinkinlabs-Projects/PatternPaint/PatternPaint/updater/
    QMAKE_OBJECTIVE_CFLAGS += -F /Users/matt/Blinkinlabs-Projects/PatternPaint/PatternPaint/updater/

    # Copy Sparkle into the app bundle
    SparkleFiles.files = /Users/matt/Blinkinlabs-Projects/PatternPaint/PatternPaint/updater/Sparkle.framework
    SparkleFiles.path = Contents/Frameworks
    QMAKE_BUNDLE_DATA += SparkleFiles

    # And add frameworks to the rpath so that the app can find the framework.
    QMAKE_RPATHDIR += @executable_path/../Frameworks
}


