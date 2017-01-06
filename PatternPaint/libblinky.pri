# TODO: How to coerce qmake to do this automatically?
INCLUDEPATH += ../libblinky

#CONFIG(debug, debug|release) {
#    LIBBLINKYDIR = ../libblinky/debug/
#}
#else {
#    LIBBLINKYDIR = ../libblinky/release/
#}
#LIBS += -L$$LIBBLINKYDIR -llibblinky

# On console buils, the 'release' and 'debug' folders don't seem to be generated
# So let's just include all the different options to be safe?
# TODO: Determine this automatically

LIBS += -llibblinky

LIBS += -L../libblinky/
LIBS += -L../libblinky/debug
LIBS += -L../libblinky/release
