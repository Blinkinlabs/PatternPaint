#!/bin/bash

# Stop at any error
set -e

# Location of the QT tools
if [ -z ${QTDIR+x} ]; then
	echo "QTDIR not defined, using default"
	QTDIR=~/Qt5.7.1/5.7/gcc_64
fi

QMAKE=${QTDIR}/bin/qmake
MAKE=make
LINUXDEPLOYQT=`pwd`/linuxdeployqt

# Location of the source tree
SOURCEDIR=`pwd`/src

# Location to build PatternPaint
BUILDDIR='build-dist-linux'


################### Extract the version info ###################
source ./gitversion.sh

################## Build PatternPaint ##########################
mkdir -p ${BUILDDIR}
pushd ${BUILDDIR}

${QMAKE} ${SOURCEDIR}/PatternPaint.pro \
    -r \
    -spec linux-g++ \
    DESTDIR=release

#${MAKE} clean
${MAKE} -j6

popd

################## Run Unit Tests ##############################
pushd ${BUILDDIR}

LD_LIBRARY_PATH=libblinky/release libblinky-test/release/libblinky-test

popd

################## Package using linuxdeployqt #################
pushd ${BUILDDIR}
pushd app/release

icns2png ${SOURCEDIR}/app/images/patternpaint.icns -x
cp patternpaint_256x256x32.png patternpaint.png

cp ${SOURCEDIR}/app/patternpaint.desktop ./

popd


# TODO: this should be done automagically though the qt build tools?
mkdir -p app/release/lib
cp libblinky/release/libblinky.so.1 app/release/lib

# linuxdeployqt requres qmake to be in the path
PATH=${QTDIR}/bin:${PATH}

unset LD_LIBRARY_PATH # Remove too old Qt from the search path; TODO: Move inside the linuxdeployqt AppImage

${LINUXDEPLOYQT} app/release/PatternPaint -verbose=2 -bundle-non-qt-libs
${LINUXDEPLOYQT} app/release/PatternPaint -verbose=2 -appimage

tar -cjf PatternPaint-x86_64_${VERSION}.tar.bz2 PatternPaint-x86_64.AppImage

popd

mv ${BUILDDIR}/PatternPaint-x86_64_${VERSION}.tar.bz2 ./
