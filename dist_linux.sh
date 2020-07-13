#!/bin/bash

# Stop at any error
set -e

# Location of the QT tools
if [ -z ${QTDIR+x} ]; then
	echo "QTDIR not defined- please set it to the location containing the Qt version to build against. For example:"
	echo "  export QTDIR=~/Qt5.8.0/5.8/gcc_64"
	exit 1
fi

QMAKE=${QTDIR}/bin/qmake
MAKE=make
LINUXDEPLOYQT=`pwd`/linuxdeployqt-continuous-x86_64.AppImage

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
    -spec linux-g++

#${MAKE} clean
${MAKE} -j6

popd

################## Run Unit Tests ##############################
pushd ${BUILDDIR}

LD_LIBRARY_PATH=libblinky libblinky-test/libblinky-test

popd

################## Package using linuxdeployqt #################
pushd ${BUILDDIR}
pushd app

icns2png ${SOURCEDIR}/app/images/patternpaint.icns -x
cp patternpaint_256x256x32.png patternpaint.png

cp ${SOURCEDIR}/app/patternpaint.desktop ./

popd


# TODO: this should be done automagically though the qt build tools?
mkdir -p app/lib
cp libblinky/libblinky.so.1 app/lib

unset LD_LIBRARY_PATH # Remove too old Qt from the search path; TODO: Move inside the linuxdeployqt AppImage

PATH=${QTDIR}/bin:${PATH} ${LINUXDEPLOYQT} app/PatternPaint -bundle-non-qt-libs
PATH=${QTDIR}/bin:${PATH} ${LINUXDEPLOYQT} app/PatternPaint -appimage

tar -cjf PatternPaint-x86_64_${VERSION}.tar.bz2 PatternPaint-x86_64.AppImage

popd
