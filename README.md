# PatternPaint

![image](https://cloud.githubusercontent.com/assets/44493/11913712/7bc560c8-a6a8-11e5-8309-70cd170a7b9e.png)

Making beautiful light shows is as easy as drawing a picture with PatternPaint! Simply plug in your BlinkyTape, run PatternPaint, and draw away!

# User Guide

This is the devlopment guide- For instructions on how to use Pattern Paint, please visit the PatternPaint website:
[http://blinkinlabs.com/patternpaint](http://blinkinlabs.com/patternpaint)

For instructions on how to contribute to Pattern Paint development, read on!

# Development

PatternPaint is a cross-platform application, targetting Mac OS X, Windows, and Linux.

We use [Github Issues](https://github.com/Blinkinlabs/PatternPaint/issues) for bug tracking and feature implementation.


## Getting started

PatternPaint is written in C++ with QT (5.4.1) libraries. The easiest way to get started is to download QT Creator, and run the project through there.

First download QT Creator 5.7.0

For OS X:

    http://download.qt.io/archive/qt/5.7/5.7.0/qt-opensource-mac-x64-clang-5.7.0.dmg

Note: for Mac users, you will also need to obtain an updated version of qtSerialPort, to get this patch: https://codereview.qt-project.org/#/c/170601/

	git clone git://code.qt.io/qt/qtserialport.git
	cd qtserialport/
	git checkout dee818e7
	~/Qt5.7.0/5.7/clang_64/bin/qmake
	make
	rm -R ~/Qt5.7.0/5.7/clang_64/lib/QtSerialPort.framework/
	mv lib/QtSerialPort.framework/ ~/Qt5.7.0/5.7/clang_64/lib/

For Windows:

    http://download.qt.io/archive/qt/5.7/5.7.0/qt-opensource-windows-x86-mingw530-5.7.0.exe
	
For Ubuntu (tested with 14.4):

	sudo apt-get install qtcreator libusb-1.0-0-dev

Next, clone the PatternPaint repository:

	git clone https://github.com/Blinkinlabs/PatternPaint.git
	
Finally, open QT creator, then open the project file PatternPaint.pro, which should be located in the PatternPaint subdirectory of the repository.

That's all you should need to do to build and make changes to PatternPaint. If you come up with a cool new feature or add support for a new device, please send a pull request!

## License

PatternPaint is licensed under the GPL version 2


# Deployment instructions

These are the steps required to build a release version (installer) for PatternPaint. This is for distribution only- for hacking or modifying PatternPaint, only the steps in the above section 'Development' are necessicary.

## OS X

### Prerequsites

Xcode (for clang compiler and git)

	https://itunes.apple.com/us/app/xcode/id497799835?ls=1&mt=12

QT dev environment from ## Getting started

Developer certificate (for signing the application)

- First, sign up for an Apple developer account and pay up to get the account. Then, in Xcode:
- xcode->preference->accounts
- add ID for dev account
- click ‘view details'
- click '+' to add ‘developer ID application’ and ‘developer ID Installer’

### Building a Pattern Paint Release

Once the prerequsites have been installed, the deployment script can be run:

	curl -O https://raw.githubusercontent.com/Blinkinlabs/PatternPaint/master/release_patternpaint_osx.sh
	sh ./release_patternpaint_osx.sh
	
If everything works, it will generate a redistributable disk image 'PatternPaint_X.Y.Z.dmg', where X.Y.Y is the current version of PatternPaint.

Test this file manually on a clean OS X host.


Note: See the script for the spicy details of making a distributable Qt app image for OS X!
Note: There might be some temporary file carnage left over after running this. Sorry about that.

## Build a release for Windows

### Prerequisites
Windows deployment requires the following tools:

Windows:
	
	Windows 7 is used internally, newer versions will likely work as well.

msysgit:

	https://git-for-windows.github.io/

Note: be sure to check 'Run Git from the Windows Command Prompt' and 'Checkout Windows-style'.

QT dev environment from ## Getting started

NSIS (3.0b2), for generating the installer:

	http://nsis.sourceforge.net/Download

Windows SDK (version 10) (for signtool):

	https://go.microsoft.com/fwlink/p/?LinkId=619296

Windows Driver Kit (version 10) (for dpinst):

	http://go.microsoft.com/fwlink/p/?LinkId=526733
	
(Optional) Dependency Walker, for tracking down which DLLs need to be included with the release:

	http://www.dependencywalker.com/
	
### Certificate setup

The release version of PatternPaint needs to be crypograpically signed. We use a certificate from GlobalSign. 

1. Install the .pfx file for code signing (right click and choose 'Install PFX'). You'll need to purchase your own if you intend to distribute PatternPaint for Windows with a signature.
2. Install the [GlobalSign cross certificate](https://jp.globalsign.com/support/docs/r1cross.cer) if you are using a GlobalSign certificate ([as suggested in David Grayson's guide](http://www.davidegrayson.com/signing/))


### Building a Pattern Paint Release

NOTE: Obtain the GlobalSign file and install the Blinkinlabs Cert before continuing. The GlobalSign file needs to be in the directory the build script is run from.

Start Git bash (start->run->git bash)

	curl -O https://raw.githubusercontent.com/Blinkinlabs/PatternPaint/master/release_patternpaint_windows.sh
	sh release_patternpaint_windows.sh


If everything works, it will generate an installer executable 'PatternPaint_Installer_X.Y.Z.exe', where X.Y.Y is the current version of PatternPaint.

Test this file manually on clean Windows 7 and 8 hosts.

## Linux

Please contact us if you have experience releasing packages for Linux. We're particularly interested in making a release for Debian-based systems. The main requirement is that a fairly recent version of Qt (5.2.1+) is available. There appear to be random bugs with each Qt release on each platform, so some attention needs to be paid to the specific version targeted.

## Create a release page on Github

1. Go to the releases page:

	https://github.com/Blinkinlabs/PatternPaint/releases
	
2. Click 'Draft a new release' to start a new release

3. Take a cool screenshot demonstrating the new features

4. Write a description of the changes

5. Upload the OS X and Windows distribution files from above

6. Pause and reflect on these changes

7. Click publish to make the release official

8. Tweet about it and maybe a blog post?

## Update the website

The PatternPaint page at blinkinlabs.com contains links to the latest releases. Update those! The page is:

http://blinkinlabs.com/patternpaint

## Update the appcast files

PatternPaint uses Sparkle and WinSparkle to notify PatternPaint users of new releases, and allow them to upgrade automatically. PattternPaint installs regularly check for updates by querying an XML file on the Blinkinlabs server. There are two files, one for OS X releases and one for Windows releases.

TODO: Script to autmate this procedure

1. Create a new &lt;item&gt; entry to describe the release. Most of the details can be scraped from the Github releases page. A sample one looks like this:

	<pre>
	&lt;item&gt;	
        &lt;title&gt;PatternPaint 1.8.0&lt;/title&gt;        
        &lt;pubDate&gt;Mon, 27 Jul 2015 8:32:00 +0000&lt;/pubDate&gt;        
        &lt;description&gt;
          &lt;![CDATA[
	&lt;b&gt;Introducing PatternPaint 1.8! This is a big one- now you can store multiple animations on your BlinkyTape at once!&lt;b&gt;
	&lt;br&gt;Changelog:&lt;br&gt;
	&lt;ul&gt;
	&lt;li&gt;New slideshow editor to load and manage multiple patterns&lt;/li&gt;
	&lt;li&gt;Upload multiple patterns to your BlinkyTape (BlinkyTile and BlinkyPendant coming soon)&lt;/li&gt;
	&lt;li&gt;Drag and drop support for loading animations directly from your file manager or favorite program&lt;/li&gt;
	&lt;li&gt;Automatically checks for new versions (using Sparkle and WinSparkle)&lt;/li&gt;
	&lt;li&gt;Mac: Appnap automatically disabled&lt;/li&gt;
	&lt;/ul&gt;
          ]]&gt;
        &lt;/description&gt;
        &lt;enclosure url="https://github.com/Blinkinlabs/PatternPaint/releases/download/1.8.0/PatternPaint_1.8.0.dmg"
          sparkle:version="1.8.0"
          length="13321443"
          type="application/octet-stream"
        /&gt;
      &lt;/item&gt;
      </pre>
      
	<pre>
	&lt;item&gt;	
        &lt;title&gt;PatternPaint 1.8.0&lt;/title&gt;        
        &lt;pubDate&gt;Mon, 27 Jul 2015 8:32:00 +0000&lt;/pubDate&gt;        
        &lt;description&gt;
          &lt;![CDATA[
	&lt;b&gt;Introducing PatternPaint 1.8! This is a big one- now you can store multiple animations on your BlinkyTape at once!&lt;b&gt;
	&lt;br&gt;Changelog:&lt;br&gt;
	&lt;ul&gt;
	&lt;li&gt;New slideshow editor to load and manage multiple patterns&lt;/li&gt;
	&lt;li&gt;Upload multiple patterns to your BlinkyTape (BlinkyTile and BlinkyPendant coming soon)&lt;/li&gt;
	&lt;li&gt;Drag and drop support for loading animations directly from your file manager or favorite program&lt;/li&gt;
	&lt;li&gt;Automatically checks for new versions (using Sparkle and WinSparkle)&lt;/li&gt;
	&lt;li&gt;Mac: Appnap automatically disabled&lt;/li&gt;
	&lt;/ul&gt;
          ]]&gt;
        &lt;/description&gt;
        &lt;enclosure url="https://github.com/Blinkinlabs/PatternPaint/releases/download/1.8.0/PatternPaint_Installer_1.8.0.exe"
          sparkle:version="1.8.0"
          length="25551480"
          type="application/octet-stream"
        /&gt;
      &lt;/item&gt;
      </pre>

2. The description and features should (probably) be the same for both OS X and Windows, so they can be created once and then copied into both files. The file links and lengths will need to be modified independently.

3. TODO: Test locally?

4. Check the appcast files into Github. Then the server can pull them from Github.

5. Upload the updated files to the Blinkinlabs server.
