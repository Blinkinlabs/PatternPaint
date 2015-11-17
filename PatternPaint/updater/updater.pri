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

    LIBS += -framework Sparkle -framework AppKit

    SPARKLE_PATH = $$PWD/../../thirdparty/Sparkle-1.11.0rc2/

    QMAKE_LFLAGS += -F $$SPARKLE_PATH
    QMAKE_OBJECTIVE_CFLAGS += -F $$SPARKLE_PATH

    # Copy Sparkle into the app bundle
    SparkleFiles.files = $$SPARKLE_PATH/Sparkle.framework
    SparkleFiles.path = Contents/Frameworks
    QMAKE_BUNDLE_DATA += SparkleFiles

    # And add frameworks to the rpath so that the app can find the framework.
    QMAKE_RPATHDIR += @executable_path/../Frameworks
}

win32:COPY_CMD = copy
unix:COPY_CMD = cp -P
macx:COPY_CMD = cp -R

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


