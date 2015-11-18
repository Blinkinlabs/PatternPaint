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

    SPARKLE_PATH = $$PWD/../../thirdparty/Sparkle-1.11.0rc2/

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

    SPARKLE_DLL = $$PWD/../../thirdparty/WinSparkle-0.4/Release/WinSparkle.dll

    INCLUDEPATH += $$PWD/../../thirdparty/WinSparkle-0.4/include/
    LIBS += $$PWD/../../thirdparty/WinSparkle-0.4/Release/WinSparkle.dll

    # Copy the Sparkle DLL into the build directory so that it can be used
    QMAKE_PRE_LINK += copy $$shell_path($$SPARKLE_DLL)  $$shell_path($$OUT_PWD) &
}


