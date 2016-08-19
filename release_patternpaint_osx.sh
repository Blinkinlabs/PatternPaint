#!/bin/bash

# Stop at any error
set -e


# Pull in the QT tools
#export QTDIR=~/Qt/5.5/clang_64/
#export QTDIR=~/Qt5.4.1/5.4/clang_64/
export QTDIR=~/Qt5.7.0/5.7/clang_64/

OUTPUTDIR=`pwd`
echo "Output to: " ${OUTPUTDIR}

# Move to a temporary directory
pushd $(mktemp -d -t PatternPaint)

BUILDDIR=`pwd`
echo "Building in: " ${BUILDDIR}

# Get the repository
git clone --depth 1 -b leoblinky https://github.com/Blinkinlabs/PatternPaint.git 
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

# Sign the frameworks
codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/Frameworks/Sparkle.framework/Versions/A
codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/Frameworks/libusb-1.0.0.dylib

# And the system frameworks 
# TODO: This is a workaround for toolchain changes in 5.5.1
#codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/Frameworks/QtCore.framework/Versions/5
#codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/Frameworks/QtDBus.framework/Versions/5
#codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/Frameworks/QtGui.framework/Versions/5
#codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/Frameworks/QtPrintSupport.framework/Versions/5
#codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/Frameworks/QtWidgets.framework/Versions/5
#codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/Frameworks/QtSerialPort.framework/Versions/5

# Deploy and sign the release
# Note: macdeployqt seems to freeze in Qt 5.5.1. Using workaround.
# TODO: This is a workaround for toolchain changes in 5.5.1
#export QTDIR=~/Qt5.4.1/5.4/clang_64/
#${QTDIR}/bin/macdeployqt PatternPaint/PatternPaint.app/ -codesign="Developer ID Application: Blinkinlabs, LLC" -dmg
cd ${QTDIR}/bin/ && ./macdeployqt ${BUILDDIR}/PatternPaint/PatternPaint/PatternPaint.app/ -codesign="Developer ID Application: Blinkinlabs, LLC" -dmg

# Perform a quick verification of the application signature
codesign --verify --verbose=4 PatternPaint/PatternPaint.app

# Test the signature
hdiutil mount PatternPaint/PatternPaint.dmg
codesign --verify --verbose=4 /Volumes/PatternPaint:PatternPaint/PatternPaint.app
umount /Volumes/PatternPaint\:PatternPaint/

# And copy out the resulting disk image
mv PatternPaint/PatternPaint.dmg ${OUTPUTDIR}/PatternPaint_${VERSION}.dmg

# TODO: Clean up temp dir
