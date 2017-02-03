#!/bin/bash

#Stop on any error
set -e


################# Signature #####################################

# Note that these are GlobalSign specific- if you want to use a
# different certificate provider, you'll need to modify these.
ROOT_CERTIFICATE='../GlobalSign_Root_CA.crt'
TIMESTAMP_SERVER='http://timestamp.globalsign.com/scripts/timstamp.dll'
#TODO
#TIMESTAMP_SERVER='http://rfc3161timestamp.globalsign.com/advanced'


################# Library Locations #############################
# Location of the QT tools
if [ -z ${QTDIR+x} ]; then
	echo "QTDIR not defined- please set it to the location containing the Qt version to build against. For example:"
	echo "  export QTDIR=/c/Qt/Qt5.8.0"
	exit 1
fi

# Location of the MINGW libraries (Installed as part of Qt)
# TODO: How to determine the correct subdir here?
QT_DIST=${QTDIR}/5.8/mingw53_32
MINGW_BIN=${QTDIR}/Tools/mingw530_32/bin

QMAKE=${QT_DIST}/bin/qmake
MAKE=${MINGW_BIN}/mingw32-make

# Project root
BASEDIR=`pwd`

# location of the source tree
SOURCEDIR=${BASEDIR}/src

# Location to build PatternPaint
BUILDDIR=${BASEDIR}/build-dist-windows

# Location of the Windows SDK and WDK
WIN_KIT_SIGNTOOL='/c/Program Files (x86)/Windows Kits/10'
WIN_KIT_DPINST='/c/Program Files (x86)/Windows Kits/8.1'

# Location of NSIS
NSIS='/c/Program Files (x86)/NSIS'

# Staging directory for assembling the installer
OUTDIR=${BUILDDIR}/bin

DRIVERS=${BASEDIR}/windows-drivers

# Blinkinlabs32u4_boards repository (for the BlinkyTape driver)
BLINKYTAPE=${DRIVERS}/Blinkinlabs32u4_boards

# BlinkyTile repository (for the LightBuddy driver)
BLINKYTILE=${DRIVERS}/Blinkytile

# BlinkyPendant repository (for the BlinkyPendant driver)
BLINKYPENDANT=${DRIVERS}/Blinkypendant

# EightByEight repository (for the EightByEight driver)
EIGHTBYEIGHT=${DRIVERS}/EightByEight

# Winsparkle library release
WINSPARKLE=${BASEDIR}/thirdparty/WinSparkle-0.5.2

# libusb library release
LIBUSB=${BASEDIR}/thirdparty/libusb-1.0.20-win



################### Extract the version info ###################
source ./gitversion.sh


################## Build PatternPaint ###################
mkdir -p ${BUILDDIR}
pushd ${BUILDDIR}

${QMAKE} ${SOURCEDIR}/PatternPaint.pro \
	-r \
	-spec win32-g++
	
#${MAKE} clean
PATH=${MINGW_BIN}:${PATH} ${MAKE} -j6

popd

################## Run Unit Tests ##############################
pushd ${BUILDDIR}

PATH=${PATH}:libblinky/release:${QT_DIST}/bin:${QT_DIST}/plugins/platforms:${WINSPARKLE}/release:${LIBUSB}/MinGW32/dll libblinky-test/release/libblinky-test

popd

################## Get device driver repositories ##############
function getRepo {
	# $1 is output directory
	# $2 is repo location

	if [ ! -d "$1" ]; then
		git clone --depth 1 $2 $1
	else
		pushd $1
		git pull
		popd
	fi

}

getRepo ${BLINKYTAPE} https://github.com/Blinkinlabs/Blinkinlabs32u4_boards.git
getRepo ${BLINKYTILE} https://github.com/Blinkinlabs/BlinkyTile.git
getRepo ${BLINKYPENDANT} https://github.com/Blinkinlabs/BlinkyPendant.git
getRepo ${EIGHTBYEIGHT} https://github.com/Blinkinlabs/EightByEight.git

################## Package Everything ############################
mkdir -p ${OUTDIR}
mkdir -p ${OUTDIR}/platforms
mkdir -p ${OUTDIR}/imageformats
mkdir -p ${OUTDIR}/driver
mkdir -p ${OUTDIR}/driver/blinkytape
mkdir -p ${OUTDIR}/driver/lightbuddy
mkdir -p ${OUTDIR}/driver/lightbuddy/x86
mkdir -p ${OUTDIR}/driver/lightbuddy/amd64
mkdir -p ${OUTDIR}/driver/blinkypendant
mkdir -p ${OUTDIR}/driver/blinkypendant/x86
mkdir -p ${OUTDIR}/driver/blinkypendant/amd64
mkdir -p ${OUTDIR}/driver/eightbyeight
mkdir -p ${OUTDIR}/driver/eightbyeight/x86
mkdir -p ${OUTDIR}/driver/eightbyeight/amd64

# Main executable
cp ${BUILDDIR}/app/release/PatternPaint.exe ${OUTDIR}

# And the libblinky library
cp ${BUILDDIR}/libblinky/release/blinky.dll ${OUTDIR}

# Note: This list of DLLs must be determined by hand, using Dependency Walker
# Also, the .nsi file should be synchronized with this list, otherwise the file
# will not actually be included by the installer.
cp ${QT_DIST}/bin/libgcc_s_dw2-1.dll ${OUTDIR}
cp ${QT_DIST}/bin/libstdc++-6.dll ${OUTDIR}
cp ${QT_DIST}/bin/libgcc_s_dw2-1.dll ${OUTDIR}
cp ${QT_DIST}/bin/libwinpthread-1.dll ${OUTDIR}

cp ${QT_DIST}/bin/Qt5Core.dll ${OUTDIR}
cp ${QT_DIST}/bin/Qt5Core.dll ${OUTDIR}
cp ${QT_DIST}/bin/Qt5Gui.dll ${OUTDIR}
cp ${QT_DIST}/bin/Qt5Widgets.dll ${OUTDIR}
cp ${QT_DIST}/bin/Qt5Gui.dll ${OUTDIR}
cp ${QT_DIST}/bin/Qt5SerialPort.dll ${OUTDIR}

cp ${QT_DIST}/plugins/platforms/qwindows.dll ${OUTDIR}/platforms/

cp ${QT_DIST}/plugins/imageformats/qgif.dll ${OUTDIR}/imageformats/
cp ${QT_DIST}/plugins/imageformats/qjpeg.dll ${OUTDIR}/imageformats/
cp ${QT_DIST}/plugins/imageformats/qsvg.dll ${OUTDIR}/imageformats/
cp ${QT_DIST}/plugins/imageformats/qtiff.dll ${OUTDIR}/imageformats/

# Winsparkle Files
cp ${WINSPARKLE}/release/WinSparkle.dll ${OUTDIR}

# libusb Files
cp ${LIBUSB}/MinGW32/dll/libusb-1.0.dll ${OUTDIR}

# BlinkyTape Driver files
cp ${BLINKYTAPE}/avr/driver/blinkinlabs.inf ${OUTDIR}/driver/blinkytape/
cp ${BLINKYTAPE}/avr/driver/blinkinlabs.cat ${OUTDIR}/driver/blinkytape/

# BlinkyTile Driver files
cp ${BLINKYTILE}/driver/lightbuddy_serial.inf ${OUTDIR}/driver/lightbuddy/
cp ${BLINKYTILE}/driver/lightbuddy_DFU_runtime.inf ${OUTDIR}/driver/lightbuddy/
cp ${BLINKYTILE}/driver/lightbuddy_DFU.inf ${OUTDIR}/driver/lightbuddy/
cp ${BLINKYTILE}/driver/libusb_device.cat ${OUTDIR}/driver/lightbuddy/
cp ${BLINKYTILE}/driver/blinkinlabs.cat ${OUTDIR}/driver/lightbuddy/
cp ${BLINKYTILE}/driver/x86/winusbcoinstaller2.dll ${OUTDIR}/driver/lightbuddy/x86/
cp ${BLINKYTILE}/driver/x86/WdfCoInstaller01009.dll ${OUTDIR}/driver/lightbuddy/x86/
cp ${BLINKYTILE}/driver/amd64/winusbcoinstaller2.dll ${OUTDIR}/driver/lightbuddy/amd64/
cp ${BLINKYTILE}/driver/amd64/WdfCoInstaller01009.dll ${OUTDIR}/driver/lightbuddy/amd64/

# BlinkyPendant Driver files
cp ${BLINKYPENDANT}/driver/blinkypendant_serial.inf ${OUTDIR}/driver/blinkypendant/
cp ${BLINKYPENDANT}/driver/blinkypendant_DFU_runtime.inf ${OUTDIR}/driver/blinkypendant/
cp ${BLINKYPENDANT}/driver/blinkypendant_DFU.inf ${OUTDIR}/driver/blinkypendant/
cp ${BLINKYPENDANT}/driver/libusb_device.cat ${OUTDIR}/driver/blinkypendant/
cp ${BLINKYPENDANT}/driver/blinkinlabs.cat ${OUTDIR}/driver/blinkypendant/
cp ${BLINKYPENDANT}/driver/x86/winusbcoinstaller2.dll ${OUTDIR}/driver/blinkypendant/x86/
cp ${BLINKYPENDANT}/driver/x86/WdfCoInstaller01009.dll ${OUTDIR}/driver/blinkypendant/x86/
cp ${BLINKYPENDANT}/driver/amd64/winusbcoinstaller2.dll ${OUTDIR}/driver/blinkypendant/amd64/
cp ${BLINKYPENDANT}/driver/amd64/WdfCoInstaller01009.dll ${OUTDIR}/driver/blinkypendant/amd64/

# EightByEight Driver files
cp ${EIGHTBYEIGHT}/driver/eightbyeight_serial.inf ${OUTDIR}/driver/eightbyeight/
cp ${EIGHTBYEIGHT}/driver/eightbyeight_DFU_runtime.inf ${OUTDIR}/driver/eightbyeight/
cp ${EIGHTBYEIGHT}/driver/eightbyeight_DFU.inf ${OUTDIR}/driver/eightbyeight/
cp ${EIGHTBYEIGHT}/driver/libusb_device.cat ${OUTDIR}/driver/eightbyeight/
cp ${EIGHTBYEIGHT}/driver/blinkinlabs.cat ${OUTDIR}/driver/eightbyeight/
cp ${EIGHTBYEIGHT}/driver/x86/winusbcoinstaller2.dll ${OUTDIR}/driver/eightbyeight/x86/
cp ${EIGHTBYEIGHT}/driver/x86/WdfCoInstaller01009.dll ${OUTDIR}/driver/eightbyeight/x86/
cp ${EIGHTBYEIGHT}/driver/amd64/winusbcoinstaller2.dll ${OUTDIR}/driver/eightbyeight/amd64/
cp ${EIGHTBYEIGHT}/driver/amd64/WdfCoInstaller01009.dll ${OUTDIR}/driver/eightbyeight/amd64/

# Driver installer
cp "${WIN_KIT_DPINST}/redist/DIFx/dpinst/MultiLin/x86/dpinst.exe" ${OUTDIR}/driver/dpinst32.exe
cp "${WIN_KIT_DPINST}/redist/DIFx/dpinst/MultiLin/x64/dpinst.exe" ${OUTDIR}/driver/dpinst64.exe


# Run NSIS to make an executablels
# For some reason the NSIS file is run from the directory it's located in?
pushd ${BUILDDIR}

cp ../patternpaint.nsi patternpaint.nsi


# Update the version info in the NSI, fail if it didn't change
sed -i 's/VERSION_STRING/'${VERSION}'/g' patternpaint.nsi
# TODO: This grep isn't working.
#grep -q "${VERSION}" patternpaint.nsi

"${NSIS}/makensis" patternpaint.nsi
rm patternpaint.nsi

if [ -z "$SIGNING_ID" ]; then
    echo "**************************************************************"
    echo "WARNING: Signing id not found, skipping code signature phase."
    echo "Resulting binary will not be signed."
    echo ""
    echo "To activate the code signature, define a SIGNING_ID envirnment"
    echo "variable before running the script. Example:"
    echo "export SIGNING_ID='Blinkinlabs, LLC'"
    echo "**************************************************************"
else
    # Sign the installer
    "${WIN_KIT_SIGNTOOL}/bin/x86/signtool" sign //v //ac ${ROOT_CERTIFICATE} //n "${SIGNING_ID}" //fd sha256 //tr ${TIMESTAMP_SERVER} //td sha256 //a PatternPaint\ Windows\ Installer.exe
fi

mv "PatternPaint Windows Installer.exe" "PatternPaint_Installer_"${VERSION}".exe"

popd

