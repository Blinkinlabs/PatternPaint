INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/autoupdater.h

SOURCES += \ 
    $$PWD/autoupdater.cpp

# For sparkle on OS X
macx {
    HEADERS +=  \
        $$PWD/sparkleautoupdater.h \
        $$PWD/cocoainitializer.h
    OBJECTIVE_SOURCES += \
        $$PWD/cocoainitializer.mm \
        $$PWD/sparkleautoupdater.mm

    LIBS += -framework AppKit

    SPARKLE_PATH = $$PWD/../../../thirdparty/Sparkle-1.17.0/

    QMAKE_LFLAGS += -F $$SPARKLE_PATH
    QMAKE_OBJECTIVE_CFLAGS += -F $$SPARKLE_PATH
    LIBS += -framework Sparkle

    # Copy Sparkle into the app bundle
    sparkle.path = Contents/Frameworks
    sparkle.files = $$SPARKLE_PATH/Sparkle.framework
    QMAKE_BUNDLE_DATA += sparkle

    # And add frameworks to the rpath so that the app can find the framework.
    QMAKE_RPATHDIR += @executable_path/../Frameworks
}

# For Winsparkle on Windows
win32 {
    HEADERS += \
        $$PWD/winsparkleautoupdater.h
    SOURCES += \
        $$PWD/winsparkleautoupdater.cpp

    SPARKLE_PATH = ../../thirdparty/WinSparkle-0.7.0
    INCLUDEPATH += $$SPARKLE_PATH/include

    win32-msvc* {
        SPARKLE_DLL = $$SPARKLE_PATH/x64/Release/WinSparkle.dll
        SPARKLE_LIB = $$SPARKLE_PATH/x64/Release/WinSparkle.lib

        LIBS += $$SPARKLE_LIB
    }
    win32-g++ {
        SPARKLE_DLL = $$SPARKLE_PATH/Release/WinSparkle.dll

        LIBS += $$SPARKLE_DLL
    }

    # Copy the Sparkle DLL into the build directory so that it can be used
    QMAKE_PRE_LINK += copy $$shell_path($$SPARKLE_DLL)  $$shell_path($$OUT_PWD) &
}


