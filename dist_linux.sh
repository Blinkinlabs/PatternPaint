#!/bin/bash

# Stop at any error
set -e

# Location of the QT tools
QTDIR=~/Qt5.7.1/5.7/gcc_64

QMAKE=${QTDIR}/bin/qmake
MAKE=make
LINUXDEPLOYQT=`pwd`/linuxdeployqt

# Location of the source tree
SOURCEDIR=`pwd`/src

# Location to build PatternPaint
BUILDDIR='build-dist-linux'

################## Get the latest linux deploy script #########

#wget -c https://github.com/probonopd/linuxdeployqt/releases/download/2/linuxdeployqt-2-x86_64.AppImage -O linuxdeployqt
#chmod a+x linuxdeployqt


################### Extract the version info ###################
source ./gitversion.sh


################## Build PatternPaint ##########################
if [ ! -d "${BUILDDIR}" ]; then
	mkdir ${BUILDDIR}
fi

pushd ${BUILDDIR}

${QMAKE} ${SOURCEDIR}/PatternPaint.pro \
    -r \
    -spec linux-g++ \
    DESTDIR=release

${MAKE} -j6


################## Run Unit Tests ##############################

LD_LIBRARY_PATH=libblinky/release libblinky-test/release/libblinky-test



################## Package using linuxdeployqt #################
pushd app/release

icns2png ${SOURCEDIR}/app/images/patternpaint.icns -x
cp patternpaint_256x256x32.png patternpaint.png

cp ${SOURCEDIR}/app/patternpaint.desktop ./

popd


# TODO: this should be done automagically though the qt build tools?
if [ ! -d "app/release/lib" ]; then
	mkdir app/release/lib
fi

cp libblinky/release/libblinky.so.1 app/release/lib


#unset LD_LIBRARY_PATH # Remove too old Qt from the search path; TODO: Move inside the linuxdeployqt AppImage


${LINUXDEPLOYQT} app/release/PatternPaint -qmldir=${SOURCEDIR}/app/PatternPaint -bundle-non-qt-libs -libpath=libblinky/release
${LINUXDEPLOYQT} app/release/PatternPaint -qmldir=${SOURCEDIR}/app/PatternPaint -appimage

# find  ./

#curl --upload-file ./*.AppImage https://transfer.sh/PatternPaint-git$(git describe --tags --always)-x86_64.AppImage

popd
