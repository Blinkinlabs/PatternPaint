#!/bin/bash

# Stop at any error
set -e

# Location of the QT tools
if [ -z ${QTDIR+x} ]; then
	echo "QTDIR not defined- please set it to the location containing the Qt version to build against. For example:"
        echo "  export QTDIR=~/Qt/5.13.0/clang_64/"
	exit 1
fi

QMAKE=${QTDIR}/bin/qmake
MAKE=make

# location of the source tree
SOURCEDIR=`pwd`'/src'

# Location to build PatternPaint
BUILDDIR=`pwd`'/build-dist-macos'

################### Extract the version info ###################
source ./gitversion.sh


################## Build PatternPaint ###################
mkdir -p ${BUILDDIR}
pushd ${BUILDDIR}

${QMAKE} ${SOURCEDIR}/PatternPaint.pro \
    -r \
    -spec macx-clang \
    CONFIG+=x86_64

#${MAKE} clean
${MAKE} -j6

popd

################## Run Unit Tests ##############################
pushd ${BUILDDIR}

# TODO: this dylib path is being pulled in incorrectly, find a way for the build to load it correctly?
install_name_tool libblinky-test/libblinky-test.app/Contents/MacOS/libblinky-test -change libblinky.1.dylib @rpath/libblinky.1.0.0.dylib

libblinky-test/libblinky-test.app/Contents/MacOS/libblinky-test

popd


################## Package using macdeployqt #################
pushd ${BUILDDIR}

APP=app/PatternPaint.app

# TODO: this dylib path is being pulled in incorrectly, find a way for the build to load it correctly?
install_name_tool ${APP}/Contents/MacOS/PatternPaint -change libblinky.1.dylib @rpath/libblinky.1.0.0.dylib

# Integrate the system frameworks
${QTDIR}/bin/macdeployqt ${APP} -verbose=1



if [ -z "$SIGNING_ID" ]; then
    echo "**************************************************************"
    echo "WARNING: Signing id not found, skipping code signature phase."
    echo "Resulting binary will not be signed."
    echo ""
    echo "To activate the code signature, define a SIGNING_ID envirnment"
    echo "variable before running the script. Example:"
    echo "export SIGNING_ID='Developer ID Application: BLINKINLABS, LLC'"
    echo "**************************************************************"

else

    # TODO: automate this instead of listing each plugin separately?
    CODESIGN_FLAGS="--verbose --force"

    # Sign the frameworks
    codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" ${APP}/Contents/Frameworks/Sparkle.framework/Versions/A
    codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" ${APP}/Contents/Frameworks/libusb-1.0.0.dylib
    codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" ${APP}/Contents/Frameworks/libblinky.1.0.0.dylib

    # And the system frameworks 
    # TODO: This is a workaround for toolchain changes in 5.5.1
    codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" ${APP}/Contents/Frameworks/QtCore.framework/Versions/5
    codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" ${APP}/Contents/Frameworks/QtGui.framework/Versions/5
    codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" ${APP}/Contents/Frameworks/QtPrintSupport.framework/Versions/5
    codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" ${APP}/Contents/Frameworks/QtSerialPort.framework/Versions/5
    codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" ${APP}/Contents/Frameworks/QtWidgets.framework/Versions/5

    # And the plugins
    codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" ${APP}/Contents/PlugIns/imageformats/libqgif.dylib
    codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" ${APP}/Contents/PlugIns/imageformats/libqicns.dylib
    codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" ${APP}/Contents/PlugIns/imageformats/libqico.dylib
    codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" ${APP}/Contents/PlugIns/imageformats/libqjpeg.dylib
    codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" ${APP}/Contents/PlugIns/imageformats/libqmacjp2.dylib
    codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" ${APP}/Contents/PlugIns/imageformats/libqtga.dylib
    codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" ${APP}/Contents/PlugIns/imageformats/libqtiff.dylib
    codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" ${APP}/Contents/PlugIns/imageformats/libqwbmp.dylib
    codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" ${APP}/Contents/PlugIns/imageformats/libqwebp.dylib

    codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" ${APP}/Contents/PlugIns/printsupport/libcocoaprintersupport.dylib

    codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" ${APP}/Contents/PlugIns/platforms/libqcocoa.dylib

    # Finally the app
    codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" ${APP}/

    # Perform a quick verification of the application signature
    codesign --deep --verify --verbose=4 ${APP}
fi

DMG_NAME=PatternPaint_${VERSION}

mkdir ${DMG_NAME}
mv ${APP}/ ${DMG_NAME}/
hdiutil create -volname ${DMG_NAME} -srcfolder ${DMG_NAME} -ov -format UDZO ${DMG_NAME}.dmg

rm -R ${DMG_NAME}

popd
