# The Ruddy Bomb!

## Getting started

1) Install platformio core
2) Install clion (using MINGW64 only)
3) Create platformio setup (follow instructions)
4) Build and use UPDATE INDEX build profile thing
5) Do everything through platformio

### Note:

Clion will not work and CMake will not compile. This __is__ intended.

Our intention is that clion is only used as a editor, not a fully fledged IDE.

Platformio will drive everything and CLion will be used for editing.

To add a library simply add it into the lib directory and rerun the index build profile thing.

![The ruddy bastards](http://rub.printmighty.co.nz/assets/Uploads/_resampled/xSetWidth846-webstorebanner4.png.pagespeed.ic.LG9ZfQe-v2.png)

## Current hardware
- Arduino Uno
- DFRobot sheild V1.0 with lcd screen

## Current libraries

- Standard Arduino
- Standard LiquidCrystal
- LCDCountDownTimer - modified with our button values
- TimerOne