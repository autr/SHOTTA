# Shotta

A quick [OpenFrameworks](https://openframeworks.cc/) app for grabbing screenshots from a video to Desktop:

![shotta](screen.png)

On MacOS it uses as fork of [ofxHAPAVPlayer](https://github.com/autr/ofxHAPAVPlayer) for fast timeline navigation, and will revert to ofVideoPlayer for other targets.

## Getting Started

Press `h` to show full help. There are two main modes - `CROP` and `RESIZE` - which use the ratio of the window but without downscaling the screenshot. The output screenshot will always be scaled to the longest size of the ratio (ie. no lossiness).
