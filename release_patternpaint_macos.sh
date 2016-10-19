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
git clone https://github.com/Blinkinlabs/PatternPaint.git
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

# Sign the frameworks
codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/Frameworks/Sparkle.framework/Versions/A
codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/Frameworks/libusb-1.0.0.dylib

# And the system frameworks 
# TODO: This is a workaround for toolchain changes in 5.5.1
codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/Frameworks/QtCore.framework/Versions/5
codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/Frameworks/QtDBus.framework/Versions/5
codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/Frameworks/QtGui.framework/Versions/5
codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/Frameworks/QtPrintSupport.framework/Versions/5
codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/Frameworks/QtSerialPort.framework/Versions/5
codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/Frameworks/QtWidgets.framework/Versions/5

# And the plugins
codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/PlugIns/imageformats/libqdds.dylib
codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/PlugIns/imageformats/libqgif.dylib
codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/PlugIns/imageformats/libqicns.dylib
codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/PlugIns/imageformats/libqico.dylib
codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/PlugIns/imageformats/libqjpeg.dylib
codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/PlugIns/imageformats/libqmacjp2.dylib
codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/PlugIns/imageformats/libqtga.dylib
codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/PlugIns/imageformats/libqtiff.dylib
codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/PlugIns/imageformats/libqwbmp.dylib
codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/PlugIns/imageformats/libqwebp.dylib

codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/PlugIns/printsupport/libcocoaprintersupport.dylib

codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/Contents/PlugIns/platforms/libqcocoa.dylib

# Finally the app
codesign --verbose --force --sign "Developer ID Application: Blinkinlabs, LLC" PatternPaint/PatternPaint.app/

# Perform a quick verification of the application signature
codesign --verify --verbose=4 PatternPaint/PatternPaint.app


# TODO: Make a dmg instead?
cd PatternPaint; zip ${OUTPUTDIR}/PatternPaint_${VERSION}.zip PatternPaint.app/ -r --symlinks

#mv PatternPaint/PatternPaint.app ${OUTPUTDIR}/PatternPaint_${VERSION}.app

# TODO: Clean up temp dir
