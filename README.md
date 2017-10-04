# The Ruddy Bomb!

## Getting started

1) Install `platformio` core via (http://docs.platformio.org/en/latest/core.html)
1) Checkout this project from github
1) Execute `platformio run` 
1) Do everything through platformio

### IDE support
Currently platformio and this project support clion. But in theory any ide with `platformio` plugin support should be fine.

Follow above instructions but don't forget to run:
`platformio init -b uno --ide clion`
 

### Note:

#### For those of you using CLion
Clion will not work and CMake will not compile. This __is__ intended.

![The ruddy bastards](http://rub.printmighty.co.nz/assets/Uploads/_resampled/xSetWidth846-webstorebanner4.png.pagespeed.ic.LG9ZfQe-v2.png)

## Current hardware
- Arduino Uno
- DFRobot sheild V1.0 with lcd screen

## Modifications to private libraries
Any modifications that are in the `/lib` directory should be listed here.

* LCDCountDownTimer - button values updated for this lcd button shield
