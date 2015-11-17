
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