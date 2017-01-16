#!/bin/bash

#Stop on any error
set -e


################# Signature #####################################

ROOT_CERTIFICATE='../GlobalSign_Root_CA.crt'
TIMESTAMP_SERVER='http://timestamp.globalsign.com/scripts/timstamp.dll'
SIGNING_ID='Blinkinlabs, LLC'


################# Library Locations #############################
# Location of the MINGW libraries (Installed as part of Qt)
QT_DIR='/c/Qt/Qt5.7.0/'
QT_MINGW=${QT_DIR}'5.7/mingw53_32/'
QT_TOOLS=${QT_DIR}'Tools/mingw530_32/bin/'
QT_REDIST=${QT_DIR}'Tools/QtCreator/bin/'

# TODO: Adjust for Win32/64?
PROGRAMFILES='/c/Program Files (x86)/'
#PROGRAMFILES='/c/Program Files/'

# Location of the Windows SDK and WDK
WIN_KIT_SIGNTOOL=${PROGRAMFILES}'Windows Kits/10/'
WIN_KIT_DPINST=${PROGRAMFILES}'Windows Kits/8.1/'

# Location of NSIS
NSIS=${PROGRAMFILES}'NSIS/'

# Location of the PatternPaint repository
PATTERNPAINT='PatternPaint/'

# Blinkinlabs32u4_boards repository (for the BlinkyTape driver)
BLINKYTAPE='Blinkinlabs32u4_boards/'

# BlinkyTile repository (for the LightBuddy driver)
BLINKYTILE='Blinkytile/'

# BlinkyPendant repository (for the BlinkyPendant driver)
BLINKYPENDANT='Blinkypendant/'

# EightByEight repository (for the EightByEight driver)
EIGHTBYEIGHT='EightByEight/'

# Winsparkle library release
WINSPARKLE=${PATTERNPAINT}'thirdparty/WinSparkle-0.5.2/'

# libusb library release
LIBUSB=${PATTERNPAINT}'thirdparty/libusb-1.0.20-win/'

# Staging directory for this release
OUTDIR='PatternPaintWindows/'

TEMPDIR='TempWindowsBuild'


VERSION='0.1.0'


################## New directory #################################
if [ ! -d "${TEMPDIR}" ]; then
	mkdir ${TEMPDIR}
else
	echo "Warning: Directory already exists, stale files may be used for build."
fi
cd ${TEMPDIR}


################## Get PatternPaint ###################
if [ ! -d "${PATTERNPAINT}" ]; then
#	git clone https://github.com/Blinkinlabs/PatternPaint.git ${PATTERNPAINT}
	git clone `pwd`/.. ${PATTERNPAINT}
else
	cd ${PATTERNPAINT}
	git pull
	cd ..
fi

################## Get BlinkyTape driver #########################
if [ ! -d "${BLINKYTAPE}" ]; then
	git clone --depth 1 https://github.com/Blinkinlabs/Blinkinlabs32u4_boards.git ${BLINKYTAPE}
else
	cd ${BLINKYTAPE}
	git pull
	cd ..
fi

################## Get BlinkyTile driver #########################
if [ ! -d "${BLINKYTILE}" ]; then
	git clone --depth 1 https://github.com/Blinkinlabs/BlinkyTile.git ${BLINKYTILE}
else
	cd ${BLINKYTILE}
	git pull
	cd ..
fi

################## Get BlinkyPendat driver #########################
if [ ! -d "${BLINKYPENDANT}" ]; then
	git clone --depth 1 https://github.com/Blinkinlabs/BlinkyPendant.git ${BLINKYPENDANT}
else
	cd ${BLINKYPENDANT}
	git pull
	cd ..
fi

################## Get EightByEight driver #########################
if [ ! -d "${EIGHTBYEIGHT}" ]; then
	git clone --depth 1 https://github.com/Blinkinlabs/EightByEight.git ${EIGHTBYEIGHT}
else
	cd ${EIGHTBYEIGHT}
	git pull
	cd ..
fi

################### Extract the version info ###################
pushd ${PATTERNPAINT}

GIT_COMMAND="git -C ${PWD}"
GIT_VERSION=`${GIT_COMMAND} describe --always --tags 2> /dev/null`
VERSION=`echo ${GIT_VERSION} | sed 's/-/\./g' | sed 's/g//g'`

echo "PatternPaint version: " ${VERSION}

popd

################## Build PatternPaint ###################
pushd ${PATTERNPAINT}src

PATH=${QT_TOOLS}:${QT_MINGW}bin/:${PATH}

qmake.exe PatternPaint.pro \
	-r \
	-spec win32-g++ \
	DESTDIR=release
	
mingw32-make.exe clean
mingw32-make.exe -j6

popd

################## Package Everything ############################
mkdir -p ${OUTDIR}
mkdir -p ${OUTDIR}platforms
mkdir -p ${OUTDIR}imageformats
mkdir -p ${OUTDIR}driver
mkdir -p ${OUTDIR}driver/blinkytape
mkdir -p ${OUTDIR}driver/lightbuddy
mkdir -p ${OUTDIR}driver/lightbuddy/x86
mkdir -p ${OUTDIR}driver/lightbuddy/amd64
mkdir -p ${OUTDIR}driver/blinkypendant
mkdir -p ${OUTDIR}driver/blinkypendant/x86
mkdir -p ${OUTDIR}driver/blinkypendant/amd64
mkdir -p ${OUTDIR}driver/eightbyeight
mkdir -p ${OUTDIR}driver/eightbyeight/x86
mkdir -p ${OUTDIR}driver/eightbyeight/amd64

# Main executable
cp ${PATTERNPAINT}src/app/release/PatternPaint.exe ${OUTDIR}

# And the libblinky library
cp ${PATTERNPAINT}src/libblinky/release/libblinky.dll ${OUTDIR}

# Note: This list of DLLs must be determined by hand, using Dependency Walker
# Also, the .nsi file should be synchronized with this list, otherwise the file
# will not actually be included by the installer.
cp ${QT_MINGW}bin/libgcc_s_dw2-1.dll ${OUTDIR}
cp ${QT_MINGW}bin/libstdc++-6.dll ${OUTDIR}
cp ${QT_MINGW}bin/libgcc_s_dw2-1.dll ${OUTDIR}
cp ${QT_MINGW}bin/libwinpthread-1.dll ${OUTDIR}

cp ${QT_MINGW}bin/Qt5Core.dll ${OUTDIR}
cp ${QT_MINGW}bin/Qt5Core.dll ${OUTDIR}
cp ${QT_MINGW}bin/Qt5Gui.dll ${OUTDIR}
cp ${QT_MINGW}bin/Qt5Widgets.dll ${OUTDIR}
cp ${QT_MINGW}bin/Qt5Gui.dll ${OUTDIR}
cp ${QT_MINGW}bin/Qt5SerialPort.dll ${OUTDIR}

cp ${QT_MINGW}plugins/platforms/qwindows.dll ${OUTDIR}platforms/

cp ${QT_MINGW}plugins/imageformats/qgif.dll ${OUTDIR}imageformats/
cp ${QT_MINGW}plugins/imageformats/qjpeg.dll ${OUTDIR}imageformats/
cp ${QT_MINGW}plugins/imageformats/qsvg.dll ${OUTDIR}imageformats/
cp ${QT_MINGW}plugins/imageformats/qtiff.dll ${OUTDIR}imageformats/

# Winsparkle Files
cp ${WINSPARKLE}/release/WinSparkle.dll ${OUTDIR}

# libusb Files
cp ${LIBUSB}/MinGW32/dll/libusb-1.0.dll ${OUTDIR}

# BlinkyTape Driver files
cp ${BLINKYTAPE}avr/driver/blinkinlabs.inf ${OUTDIR}driver/blinkytape/
cp ${BLINKYTAPE}avr/driver/blinkinlabs.cat ${OUTDIR}driver/blinkytape/

# BlinkyTile Driver files
cp ${BLINKYTILE}driver/lightbuddy_serial.inf ${OUTDIR}driver/lightbuddy/
cp ${BLINKYTILE}driver/lightbuddy_DFU_runtime.inf ${OUTDIR}driver/lightbuddy/
cp ${BLINKYTILE}driver/lightbuddy_DFU.inf ${OUTDIR}driver/lightbuddy/
cp ${BLINKYTILE}driver/libusb_device.cat ${OUTDIR}driver/lightbuddy/
cp ${BLINKYTILE}driver/blinkinlabs.cat ${OUTDIR}driver/lightbuddy/
cp ${BLINKYTILE}driver/x86/winusbcoinstaller2.dll ${OUTDIR}driver/lightbuddy/x86/
cp ${BLINKYTILE}driver/x86/WdfCoInstaller01009.dll ${OUTDIR}driver/lightbuddy/x86/
cp ${BLINKYTILE}driver/amd64/winusbcoinstaller2.dll ${OUTDIR}driver/lightbuddy/amd64/
cp ${BLINKYTILE}driver/amd64/WdfCoInstaller01009.dll ${OUTDIR}driver/lightbuddy/amd64/

# BlinkyPendant Driver files
cp ${BLINKYPENDANT}driver/blinkypendant_serial.inf ${OUTDIR}driver/blinkypendant/
cp ${BLINKYPENDANT}driver/blinkypendant_DFU_runtime.inf ${OUTDIR}driver/blinkypendant/
cp ${BLINKYPENDANT}driver/blinkypendant_DFU.inf ${OUTDIR}driver/blinkypendant/
cp ${BLINKYPENDANT}driver/libusb_device.cat ${OUTDIR}driver/blinkypendant/
cp ${BLINKYPENDANT}driver/blinkinlabs.cat ${OUTDIR}driver/blinkypendant/
cp ${BLINKYPENDANT}driver/x86/winusbcoinstaller2.dll ${OUTDIR}driver/blinkypendant/x86/
cp ${BLINKYPENDANT}driver/x86/WdfCoInstaller01009.dll ${OUTDIR}driver/blinkypendant/x86/
cp ${BLINKYPENDANT}driver/amd64/winusbcoinstaller2.dll ${OUTDIR}driver/blinkypendant/amd64/
cp ${BLINKYPENDANT}driver/amd64/WdfCoInstaller01009.dll ${OUTDIR}driver/blinkypendant/amd64/

# EightByEight Driver files
cp ${EIGHTBYEIGHT}driver/eightbyeight_serial.inf ${OUTDIR}driver/eightbyeight/
cp ${EIGHTBYEIGHT}driver/eightbyeight_DFU_runtime.inf ${OUTDIR}driver/eightbyeight/
cp ${EIGHTBYEIGHT}driver/eightbyeight_DFU.inf ${OUTDIR}driver/eightbyeight/
cp ${EIGHTBYEIGHT}driver/libusb_device.cat ${OUTDIR}driver/eightbyeight/
cp ${EIGHTBYEIGHT}driver/blinkinlabs.cat ${OUTDIR}driver/eightbyeight/
cp ${EIGHTBYEIGHT}driver/x86/winusbcoinstaller2.dll ${OUTDIR}driver/eightbyeight/x86/
cp ${EIGHTBYEIGHT}driver/x86/WdfCoInstaller01009.dll ${OUTDIR}driver/eightbyeight/x86/
cp ${EIGHTBYEIGHT}driver/amd64/winusbcoinstaller2.dll ${OUTDIR}driver/eightbyeight/amd64/
cp ${EIGHTBYEIGHT}driver/amd64/WdfCoInstaller01009.dll ${OUTDIR}driver/eightbyeight/amd64/

# Driver installer
cp "${WIN_KIT_DPINST}redist/DIFx/dpinst/MultiLin/x86/dpinst.exe" ${OUTDIR}driver/dpinst32.exe
cp "${WIN_KIT_DPINST}redist/DIFx/dpinst/MultiLin/x64/dpinst.exe" ${OUTDIR}driver/dpinst64.exe

# Run NSIS to make an executablels
# For some reason the NSIS file is run from the directory it's located in?
cp ${PATTERNPAINT}"Pattern Paint.nsi" "Pattern Paint.nsi"

# Update the version info in the NSI, fail if it didn't change
sed -i 's/VERSION_STRING/'${VERSION}'.0/g' "Pattern Paint.nsi"
grep -q ${VERSION} "Pattern Paint.nsi"

"${NSIS}makensis.exe" "Pattern Paint.nsi"

rm "Pattern Paint.nsi"

# Sign the installer
"${WIN_KIT_SIGNTOOL}bin/x86/signtool.exe" sign //v //ac ${ROOT_CERTIFICATE} //n "${SIGNING_ID}" //fd sha256 //tr ${TIMESTAMP_SERVER} //td sha256 //a PatternPaint\ Windows\ Installer.exe

mv "PatternPaint Windows Installer.exe" "../PatternPaint_Installer_"${VERSION}".exe"
