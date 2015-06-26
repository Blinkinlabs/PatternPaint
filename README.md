# PatternPaint

![image](http://blinkinlabs.com/wp-content/uploads/2014/07/Screen-Shot-2015-03-31-at-10.33.14-AM.png)

Making beautiful light shows is as easy as drawing a picture with Pattern Paint! Simply plug in your BlinkyTape, run Pattern Paint, and draw away!

For instructions, see:
[http://blinkinlabs.com/patternpaint](http://blinkinlabs.com/patternpaint)

# Developing

PatternPaint is written in C++ with QT (5.4.1) libraries. The easiest way to get started is to download QT Creator and use it to open the project file:
http://download.qt.io/archive/qt/5.4/5.4.1/


# Build and deployment instructions for OS X:

## Prerequsites

Xcode (for clang compiler and git)

	Find and install through the App Store

QT dev environment (Qt 5.4.1 for Mac):

	http://download.qt.io/archive/qt/5.4/5.4.1/qt-opensource-mac-x64-clang-5.4.1.dmg

Developer certificate (optional, for signing the application)

- First, sign up for an Apple developer account and pay up to get the account. Then, in Xcode:
- xcode->preference->accounts
- add ID for dev account
- click ‘view details'
- click '+' to add ‘developer ID application’ and ‘developer ID Installer’

## Manual Build

1.	Download the repository:

	git clone https://github.com/Blinkinlabs/PatternPaint.git

2.	Set up the QT environment:

	export QTDIR=~/Qt5.4.1/5.4/clang_64/

3.	Build PatternPaint:

	cd PatternPaint/PatternPaint
	rm -R build/
	${QTDIR}/bin/qmake MOC_DIR=build OBJECTS_DIR=build RCC_DIR=build UI_DIR=build 	DESTDIR=bin VERSION=1.6.0
	make
	cd ..
	
4.	Use the bundler tool to make a .dmg file, signing it in the process:

	${QTDIR}/bin/macdeployqt PatternPaint/bin/PatternPaint.app/ -codesign="Developer ID Application: xxxx" -dmg
	codesign --verify --verbose=4 PatternPaint/bin/PatternPaint.app

5.	Test the signature:

	hdiutil mount PatternPaint/bin/PatternPaint.dmg
	codesign --verify --verbose=4 /Volumes/PatternPaint:bin:PatternPaint/PatternPaint.app
	umount /Volumes/PatternPaint\:bin\:PatternPaint/

6.	Change the dmg name to include the version, for example PatternPaint_0.1.0.dmg

	mv PatternPaint/bin/PatternPaint.dmg ./PatternPaint_1.6.1.dmg

7.	Distribute!

# Build and deployment instructions for Windows:

## Prerequisites
Windows deployment requires the following tools:

msysgit:

	http://msysgit.github.io/

Note: be sure to check 'Run Git from the Windows Command Prompt' and 'Checkout Windows-style'.

The QT dev environment (Qt 5.3.2 for Windows 32-bit (MinGW 4.8.2, OpenGL):

	http://download.qt-project.org/official_releases/qt/5.3/5.3.2/qt-opensource-windows-x86-mingw482_opengl-5.3.2.exe

NSIS (3.0b1), for generating the installer:

	http://nsis.sourceforge.net/Download

Dependency Walker, for tracking down which DLLs need to be included with the release:

	http://www.dependencywalker.com/

Windows SDK (version 8.1):

	http://www.microsoft.com/click/services/Redirect2.ashx?CR_EAC=300135395

Windows Driver Kit (version 8.0):

	http://go.microsoft.com/fwlink/p/?LinkID=324284

NOTE: Obtain the GlobalSign file and install the Blinkinlabs Cert before continuing.

## Building a Pattern Paint Release

### Automatic build
Run BuildPatternPaint_Windows.sh (do this using Git Bash, start->run->git bash)

### Manual steps (if the automatic build fails)

1.	Clone the PatternPaint repo:

	git clone https://github.com/Blinkinlabs/PatternPaint.git

2.	Clone 32u4 support repo (for the blinkytape driver):

	git clone https://github.com/Blinkinlabs/Blinkinlabs32u4_boards.git


3.	Build PatternPaint:

	cd PatternPaint
	qmake -config release MOC_DIR=build OBJECTS_DIR=build RCC_DIR=build UI_DIR=build DESTDIR=bin VERSION=0.1.0
	mingw32-make


4.	Copy over the dlls and driver files

NOTE: These files were determined by using the dependency walker tool.
NOTE: qwindows.dll wasn't detected by dependency walker for some reason.

	:: Location of the MINGW libraries (Installed as part of Qt)
	set QT_MINGW="c:\Qt\Qt5.1.1\5.1.1\mingw48_32\"

	:: Location of the BlinkyTape driver (installed as our repository)
	set DRIVER="..\..\..\Blinkinlabs32u4_boards\driver\"

	:: Location of the Windows SDK and WDK
	set WIN_KIT="c:\Program Files (x86)\Windows Kits\8.1\"

	cd bin
	copy %QT_MINGW%bin\Qt5Core.dll
	copy %QT_MINGW%bin\Qt5Core.dll
	copy %QT_MINGW%bin\Qt5Gui.dll
	copy %QT_MINGW%bin\Qt5SerialPort.dll
	copy %QT_MINGW%bin\Qt5Widgets.dll
	copy %QT_MINGW%bin\Qt5Guilibgcc_s_dw2-1.dll
	copy %QT_MINGW%bin\libstdc++-6.dll
	copy %QT_MINGW%bin\icuin51.dll
	copy %QT_MINGW%bin\icuuc51.dll
	copy %QT_MINGW%bin\icudt51.dll
	copy %QT_MINGW%bin\libgcc_s_dw2-1.dll
	copy %QT_MINGW%bin\libwinpthread-1.dll

	mkdir platforms
	copy %QT_MINGW%\plugins\platforms\qwindows.dll platforms\

	mkdir driver
	copy %DRIVER%blinkinlabs.inf driver\
	copy %DRIVER%blinkinlabs.cat driver\
	copy %WIN_KIT%"redist\DIFx\dpinst\MultiLin\x86\dpinst.exe" driver\dpinst32.exe
	copy %WIN_KIT%"redist\DIFx\dpinst\MultiLin\x64\dpinst.exe" driver\dpinst64.exe

	cd ..

5.	Run the NSIS installer to generate a .exe file

NOTE: The initial .nsi file was created using "NSIS Quick Setup Script Generator". The output of this file was modified to incorporate the driver installer

6.	Sign the .exe file

	"C:\Program Files (x86)\Windows Kits\8.1\bin\x86\signtool" sign /v /ac "GlobalSign Root CA.crt" /n "Blinkinlabs, LLC" /tr http://tsa.starfieldtech.com *.cat


7.	Enjoy! Or, enjoy testing the build on the myriad flavors of Windows!
