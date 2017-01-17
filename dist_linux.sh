#!/bin/bash

# Stop at any error
set -e

# Location of the QT tools
QTDIR=~/Qt5.7.1/5.7/gcc_64

# Location to build PatternPaint
SHADOWBUILD='build-dist-linux/'

QMAKE=${QTDIR}/bin/qmake
MAKE=make

################## Get the latest linux deploy script #########

#wget -c https://github.com/probonopd/linuxdeployqt/releases/download/2/linuxdeployqt-2-x86_64.AppImage -O linuxdeployqt
#chmod a+x linuxdeployqt


################### Extract the version info ###################
source ./gitversion.sh


################## Build PatternPaint ###################
if [ ! -d "${SHADOWBUILD}" ]; then
	mkdir ${SHADOWBUILD}
fi

pushd ${SHADOWBUILD}

${QMAKE} ../src/PatternPaint.pro \
    -r \
    -spec linux-g++ \
    DESTDIR=release

${MAKE} -j6

# Run unit tests
#LD_LIBRARY_PATH=./libblinky libblinky-test/libblinky-test

popd

################## Package ##############################
#icns2png app/images/patternpaint.icns -x
#cp patternpaint_256x256x32.png patternpaint.png
#cd ..
#- find src/


#unset LD_LIBRARY_PATH # Remove too old Qt from the search path; TODO: Move inside the linuxdeployqt AppImage
#./linuxdeployqt src/app/PatternPaint -qmldir=src/app/PatternPaint -bundle-non-qt-libs -libpath=src/libblinky
#./linuxdeployqt src/app/PatternPaint -qmldir=src/app/PatternPaint -appimage -libpath=src/libblinky


#curl --upload-file ./*.AppImage https://transfer.sh/PatternPaint-git$(git describe --tags --always)-x86_64.AppImage
