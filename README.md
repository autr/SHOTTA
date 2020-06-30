# SHOTTA

SHOTTA is a simple app for quickly grabbing screenshots from a video to Desktop. 

![shotta](screen.png)

On MacOS it uses as fork of [ofxHAPAVPlayer](https://github.com/autr/ofxHAPAVPlayer) for nicer navigation of the timeline (skipping, playback, randomised positions), but it should also compile for Windows/Linux without much fuss.

## Cropping and Scaling

There are two modes: CROP and RESIZE, which use the ratio of the window to either: crop or toggle. The pixel size of the window won’t effect the output resolution, and the app will work out the most “non-destructive” resolution to output as (ie. longest side).

## Instructions

```
To change the ratio (4:3, 16:9, etc) press 1 - 6
To toggle CROP / RESIZE press Tab
To take a screen grab press C (outputs to the Desktop)
To randomise press R
To skip around on the timeline, press LEFT and RIGHT (and with SHIFT)
```
