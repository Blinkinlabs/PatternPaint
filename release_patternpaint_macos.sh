#!/bin/bash

# Stop at any error
set -e

# Pull in the QT tools
export QTDIR=~/Qt5.7.0/5.7/clang_64/

OUTPUTDIR=`pwd`
echo "Output to: " ${OUTPUTDIR}

# Move to a temporary directory
pushd $(mktemp -d -t com.blinkinlabs.PatternPaint)
echo "Building in: " `pwd`

# Get the repository
#git clone https://github.com/Blinkinlabs/PatternPaint.git
git clone ${OUTPUTDIR}/.git
cd PatternPaint/PatternPaint

# Extract the version
GIT_COMMAND="git -C ${PWD}"
GIT_VERSION=`${GIT_COMMAND} describe --always --tags 2> /dev/null`
VERSION=`echo ${GIT_VERSION} | sed 's/-/\./g' | sed 's/g//g'`

echo "PatternPaint version: " ${VERSION}

# Build PatternPaint
${QTDIR}/bin/qmake -config release OBJECTS_DIR=build MOC_DIR=build/moc RCC_DIR=build/rcc UI_DIR=build/uic
make -j
cd ..

# Integrate the system frameworks
${QTDIR}/bin/macdeployqt PatternPaint/PatternPaint.app -verbose=1

# TODO: automate this instead of listing each plugin separately?

SIGNING_ID='Developer ID Application: BLINKINLABS, LLC (9NW4AWMNS6)'
CODESIGN_FLAGS="--verbose --force"

# Sign the frameworks
codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" PatternPaint/PatternPaint.app/Contents/Frameworks/Sparkle.framework/Versions/A
codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" PatternPaint/PatternPaint.app/Contents/Frameworks/libusb-1.0.0.dylib

# And the system frameworks 
# TODO: This is a workaround for toolchain changes in 5.5.1
codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" PatternPaint/PatternPaint.app/Contents/Frameworks/QtCore.framework/Versions/5
codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" PatternPaint/PatternPaint.app/Contents/Frameworks/QtDBus.framework/Versions/5
codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" PatternPaint/PatternPaint.app/Contents/Frameworks/QtGui.framework/Versions/5
codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" PatternPaint/PatternPaint.app/Contents/Frameworks/QtPrintSupport.framework/Versions/5
codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" PatternPaint/PatternPaint.app/Contents/Frameworks/QtSerialPort.framework/Versions/5
codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" PatternPaint/PatternPaint.app/Contents/Frameworks/QtWidgets.framework/Versions/5

# And the plugins
codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" PatternPaint/PatternPaint.app/Contents/PlugIns/imageformats/libqdds.dylib
codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" PatternPaint/PatternPaint.app/Contents/PlugIns/imageformats/libqgif.dylib
codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" PatternPaint/PatternPaint.app/Contents/PlugIns/imageformats/libqicns.dylib
codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" PatternPaint/PatternPaint.app/Contents/PlugIns/imageformats/libqico.dylib
codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" PatternPaint/PatternPaint.app/Contents/PlugIns/imageformats/libqjpeg.dylib
codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" PatternPaint/PatternPaint.app/Contents/PlugIns/imageformats/libqmacjp2.dylib
codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" PatternPaint/PatternPaint.app/Contents/PlugIns/imageformats/libqtga.dylib
codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" PatternPaint/PatternPaint.app/Contents/PlugIns/imageformats/libqtiff.dylib
codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" PatternPaint/PatternPaint.app/Contents/PlugIns/imageformats/libqwbmp.dylib
codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" PatternPaint/PatternPaint.app/Contents/PlugIns/imageformats/libqwebp.dylib

codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" PatternPaint/PatternPaint.app/Contents/PlugIns/printsupport/libcocoaprintersupport.dylib

codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" PatternPaint/PatternPaint.app/Contents/PlugIns/platforms/libqcocoa.dylib

# Finally the app
codesign ${CODESIGN_FLAGS} --sign "${SIGNING_ID}" PatternPaint/PatternPaint.app/

# Perform a quick verification of the application signature
codesign --verify --verbose=4 PatternPaint/PatternPaint.app


# TODO: Make a dmg instead?
#cd PatternPaint; zip ${OUTPUTDIR}/PatternPaint_${VERSION}.zip PatternPaint.app/ -r --symlinks

#mv PatternPaint/PatternPaint.app ${OUTPUTDIR}/PatternPaint_${VERSION}.app

DMG_NAME=PatternPaint_${VERSION}

mkdir ${DMG_NAME}
mv PatternPaint/PatternPaint.app/ ${DMG_NAME}/
hdiutil create -volname ${DMG_NAME} -srcfolder ${DMG_NAME} -ov -format UDZO ${OUTPUTDIR}/${DMG_NAME}.dmg



# TODO: Clean up temp dir
