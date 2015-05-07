################## Library Locations #############################
# Location of the MINGW libraries (Installed as part of Qt)
QT_DIR='/c/Qt/'
QT_MINGW=${QT_DIR}'/5.4/mingw491_32/'
QT_TOOLS=${QT_DIR}'/Tools/mingw491_32/bin/'

# Location of the Windows SDK and WDK
WIN_KIT='/c/Program Files (x86)/Windows Kits/8.0/'
WIN_KIT2='/c/Program Files (x86)/Windows Kits/8.1/'

# Location of NSIS
NSIS='/c/Program Files (x86)/NSIS'

# Location of the PatternPaint repository
PATTERNPAINT='PatternPaint'

# Blinkinlabs32u4_boards repository (for the BlinkyTape driver)
BLINKYTAPE='Blinkinlabs32u4_boards/'

# BlinkyTile repository (for the LightBuddy driver)
BLINKYTILE='Blinkytile/'

# Staging directory for this release
OUTDIR='PatternPaintWindows/'


################## Pull PatternPaint and build ###################
git clone https://github.com/Blinkinlabs/PatternPaint.git ${PATTERNPAINT}

# TODO: Detect if it's already installed?
cd ${PATTERNPAINT}
git pull

PATH=${QT_TOOLS}:${PATH}

${QT_MINGW}bin/qmake.exe -config release MOC_DIR=build OBJECTS_DIR=build RCC_DIR=build UI_DIR=build DESTDIR=bin VERSION=1.5.0
mingw32-make.exe clean
mingw32-make.exe -j 4

cd ..

################## Get BlinkyTape driver #########################
git clone https://github.com/Blinkinlabs/Blinkinlabs32u4_boards.git ${BLINKYTAPE}

cd ${BLINKYTAPE}
git pull
cd ..

################## Get BlinkyTile driver #########################
git clone https://github.com/Blinkinlabs/BlinkyTile.git ${BLINKYTILE}

cd ${BLINKYTILE}
git pull
cd ..

################## Package Everything ############################
mkdir -p ${OUTDIR}
mkdir -p ${OUTDIR}platforms
mkdir -p ${OUTDIR}imageformats
mkdir -p ${OUTDIR}driver
mkdir -p ${OUTDIR}driver/blinkytape
mkdir -p ${OUTDIR}driver/lightbuddy
mkdir -p ${OUTDIR}driver/lightbuddy/x86
mkdir -p ${OUTDIR}driver/lightbuddy/amd64

# Main executable
cp ${PATTERNPAINT}/bin/PatternPaint.exe ${OUTDIR}

# Note: This list of DLLs must be determined by hand, using Dependency Walker
# Also, the .nsi file should be synchronized with this list, otherwise the file
# will not actually be included by the installer.
cp ${QT_MINGW}bin/Qt5Core.dll ${OUTDIR}
cp ${QT_MINGW}bin/Qt5Core.dll ${OUTDIR}
cp ${QT_MINGW}bin/Qt5Gui.dll ${OUTDIR}
cp ${QT_MINGW}bin/Qt5SerialPort.dll ${OUTDIR}
cp ${QT_MINGW}bin/Qt5Widgets.dll ${OUTDIR}
cp ${QT_MINGW}bin/Qt5Gui.dll ${OUTDIR}
cp ${QT_MINGW}bin/libgcc_s_dw2-1.dll ${OUTDIR}
cp ${QT_MINGW}bin/libstdc++-6.dll ${OUTDIR}
cp ${QT_MINGW}bin/icuin53.dll ${OUTDIR}
cp ${QT_MINGW}bin/icuuc53.dll ${OUTDIR}
cp ${QT_MINGW}bin/icudt53.dll ${OUTDIR}
cp ${QT_MINGW}bin/libgcc_s_dw2-1.dll ${OUTDIR}
cp ${QT_MINGW}bin/libwinpthread-1.dll ${OUTDIR}

cp ${QT_MINGW}/plugins/platforms/qwindows.dll ${OUTDIR}platforms/

cp ${QT_MINGW}/plugins/imageformats/qgif.dll ${OUTDIR}imageformats/
cp ${QT_MINGW}/plugins/imageformats/qjpeg.dll ${OUTDIR}imageformats/
cp ${QT_MINGW}/plugins/imageformats/qsvg.dll ${OUTDIR}imageformats/
cp ${QT_MINGW}/plugins/imageformats/qtiff.dll ${OUTDIR}imageformats/

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

# Driver installer
cp "${WIN_KIT}redist/DIFx/dpinst/MultiLin/x86/dpinst.exe" ${OUTDIR}driver/dpinst32.exe
cp "${WIN_KIT}redist/DIFx/dpinst/MultiLin/x64/dpinst.exe" ${OUTDIR}driver/dpinst64.exe

# Run NSIS to make an executable
"${NSIS}/makensis.exe" "NSIS/Pattern Paint.nsi"

# Sign the installer
# NOTE: You need to install the Blinkinlabs key and the GlobalSign Root CA for this to work
"${WIN_KIT2}bin/x86/signtool.exe" sign //v //ac "GlobalSign Root CA.crt" //n "Blinkinlabs, LLC" //tr http://tsa.starfieldtech.com "PatternPaint Windows Installer.exe"

