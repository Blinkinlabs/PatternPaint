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

Start Git bash (start->run->git bash)

	BuildPatternPaint_Windows.sh
	
Note: if the automatic build fails, it's probably because a version number of a file changed slightly. This is a fairly brittle build system.
