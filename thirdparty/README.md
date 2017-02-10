# Third party libraries used in Pattern Paint
This directory contains third-party libraries that are distributed along with the Pattern Paint exectuable. They are organized by platform

# macOS

## Sparkle
Pattern Paint integrates [Sparkle](http://sparkle-project.org/) for automatic app updates on OS X.

### Obtaining
Grab version 1.14:
[https://github.com/sparkle-project/Sparkle/releases/download/1.14.0/Sparkle-1.14.0.tar.bz2](https://github.com/sparkle-project/Sparkle/releases/download/1.14.0/Sparkle-1.14.0.tar.bz2)

### Building
TODO: Instructions for how to update the rpath of the framework (?)

## LibUSB
Pattern paint uses 

### Obtaining

### Building

For simplicity, the compiled binaries are included in the github repo. These were the steps taken to produce them:

	cd libusb-1.0.20
	./configure --prefix $PWD/osx-install
	make
	make install
	install_name_tool -id @rpath/libusb-1.0.0.dylib osx-install/lib/libusb-1.0.0.dylib

# Windows

## WinSparkle
[WinSparkle](http://winsparkle.org) is used for automatic app updates on the Windows platform.

### Obtaining
Grab WinSparkle:
[https://github.com/vslavik/winsparkle/releases](https://github.com/vslavik/winsparkle/releases/)

### Building
No special steps need to be performed to use WinSparkle. The download contains a .dll that can be used directly. The file '../src/app/updater/updater.pri' will need to be updated if the version number changes.


## Libusb
Pattern paint uses [libusb](libusb.info) for gathering information about each installed USB device. 

### Obtaining

We're using [libusb-1.0.20](http://sourceforge.net/projects/libusb/files/libusb-1.0/libusb-1.0.20/). Get the [release file](http://sourceforge.net/projects/libusb/files/libusb-1.0/libusb-1.0.20/libusb-1.0.20.7z/download), unzip it, then move it to thirdparty/libusb-1.0.20-win.

### Building

No special steps need to be performed to use libusb. The download contains a .dll that can be used directly.

# Linux

## LibUSB
TODO: This should come with most distros, but might need to be installed?
