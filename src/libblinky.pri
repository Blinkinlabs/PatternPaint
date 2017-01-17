CONFIG(release, debug|release): LIBBLINKYDIR=$$OUT_PWD/../libblinky/release/
else:CONFIG(debug, debug|release): LIBBLINKYDIR=$$OUT_PWD/../libblinky/debug/

INCLUDEPATH += $$PWD/libblinky
DEPENDPATH += $$PWD/libblinky

LIBS += -L$$LIBBLINKYDIR -lblinky
