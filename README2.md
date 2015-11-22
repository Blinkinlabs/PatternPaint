
# Pattern Paint

## Scene
A scene is a configuration of at least one Controller and one Fixture. An example is the BlinkyTape- it has one BlinkyTape control board, and one strip of 60 WS2812 LEDs. Scenes can either be built-in (BlinkyTape, BlinkyPendant, BlinkyPixels, etc), or custom (BlinkyTape Controller + externally purchased LED strip)

Built-in scenes are:

* [BlinkyTape](http://shop.blinkinlabs.com/products/blinkytape-basic) (BlinkyTape controller + 1m LED strip with 60 LEDs)
* [Blinky Pixels](http://shop.blinkinlabs.com/products/blinky-pixels) (BlinkyTape controller + 50 LED pixels)
* [BlinkyMatrix](http://shop.blinkinlabs.com/products/blinky-matrix) (BlinkyTape controller + flexible 8x8 grid of LEDs)
* [BlinkyTile](http://shop.blinkinlabs.com/products/blinkytile-kit) (BlinkyTile controller + 1-140 BlinkyTile pieces)
* [BlinkyPendant](http://shop.blinkinlabs.com/products/blinky-pendant) (BlinkyPendant controller + 10 RGB LEDs)
* Custom device: Any of the below controllers connected to a compatible fixture!

## Controller
A controller is a device that can control LEDs. An example is the BlinkyTape control board, which is a small microcontroller board built into the BlinkyTape, Blinky Pixels, and BlinkyMatrix. Controllers may have the capability to display patterns from the computer in real time, play back one or more patterns from a storage, or both. The controller can have one or more Fixtures attached to it.

Supported controllers are:

* [BlinkyTape control board](http://shop.blinkinlabs.com/products/blinkytape-control-board)
* BlinkyTile controller
* [BlinkyPendant](http://shop.blinkinlabs.com/products/blinky-pendant)
* Arduino Leonardo (yes!)

## Fixture
A fixture is an arrangement of LEDs. Pattern paint supports the following fixture types:

* Linear: Single strip of LEDs spaced evenly along a row (BlinkyTape)
* Matrix: A grid of LEDs (BlinkyMatrix)
* Arbitrary: 2d arrangement of LEDs in any arbitrary posisitons (BlinkyTile, BlinkyPixels) (coming soon)

# Patterns

## Scrolling pattern
A scrolling pattern consists of a static image file, which is made into an animation by sliding the fixture over it. This kind of pattern is suited for light painting, POV displays, and scrolling messages over an LED matrix.

## Frame-based pattern
Frame-based patterns consist of a set of frames, such as a GIF animation or video. The animation is made by drawing each frame onto the fixture in succession, like projecting a movie onto the frame surface. This kind of pattern is suited for animations on 2 or 3D displays.

# BlinkyTape Protocol

The BlinkyTape protocol describes how to send data to the BlinkyTape (and compatible) controllers over a serial link. Data is sent as RGB triplets, with 8 bits per color channel. Color can be in the range of 0-254. A 255 character signals a frame ending.

## Example: 3 color LED device

Say we want to send data to a string of 3 LEDs attached to a controller. We want the first LED to show red, the second green, and the third white. The data stream would look like this:

[254][0][0][0][254][0][254][254][254][255]

The first three bytes are RGB for the first pixel (254,0,0) = red. The next three bytes are RGB for the second pixel (0,254,0) = green. The following three bytes are RGB for the third pixel (254,254,254) = white. Finally, a (255) = break character is added at the end to signal that the frame should be drawn.

## Notes

For devices where the color order is changed, such as Blinky Pixel, the data stream should be swapped before sending it to the controller. At this time, only 8 bit, 3 color outputs are supported by this protocol. It will need to be modified to support RGBW in the future.

# Uploading to a BlinkyTape

Patterns are uploaded to a BlinkyTape by putting it into bootloader mode, instructing the bootloader to erase the flash, then uploading a new application firmware with the patterns appended to it. PatternPaint emulates a simple AVR109-compatible bootloader protocol in order to support this.

The BlinkyTape controller memory map looks like this:

	[0x0000] Start of application firmware (PatternPlayer_Sketch)
	[0x????] Start of pattern memory (first page after the application firmware)
	[0x6F80] Pattern Table
	[0x7000] Start of bootloader (Caterina)

The pattern Table looks like this:

	[0x0000] Number of patterns stored (1 byte, 0-255)
	[0x0001] Number of LEDs attached to the controller (2 bytes, 0-65535)
	[0x0003] First Pattern Entry
	[0x000A] Second Pattern Entry
	[0x0011] Third Pattern Entry