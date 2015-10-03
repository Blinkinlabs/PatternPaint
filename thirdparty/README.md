# Third party libraries used in Pattern Paint
This directory contains third-party libraries that are distributed along with the Pattern Paint exectuable. They are organized by platform

# Mac OS X

## Sparkle
Pattern Paint integrates [Sparkle](http://sparkle-project.org/) for automatic app updates on OS X.

### Obtaining
Grab version 1.11.0rc1:
[https://github.com/sparkle-project/Sparkle/releases/tag/1.11.0rc1](https://github.com/sparkle-project/Sparkle/releases/tag/1.11.0rc1)

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
Grab WinSparkle 1.4:
[https://github.com/vslavik/winsparkle/releases/tag/v0.4](https://github.com/vslavik/winsparkle/releases/tag/v0.4)

### Building
No special steps need to be performed to use WinSparkle. The download contains a .dll that can be used directly.

## LibUSB
TODO

# Linux

## LibUSB
TODO
