# The Ruddy Bomb!

![The ruddy bastards](http://rub.printmighty.co.nz/assets/Uploads/_resampled/xSetWidth846-webstorebanner4.png.pagespeed.ic.LG9ZfQe-v2.png)

## The goal of this project.

To create a simple and reliable Airsoft game device that enhances play by introducing electronically automated objectives and measured team performances.  

Assembled with an Arduino board and other off the shelf components, it is designed to be built by anyone with limited electronics knowledge. 

The interface is as simple and intuitive as possible, lowering the learning curve and creating a device that is easy to use on the field.

In an attempt to encourage wide adoption we have put focus on keeping the production cost low, making the code open source and having the assembly process as simple as possible.

## What does it do?

It simulates a well known game type called Search and Destroy. The device is planted on the field by one team, and then is attempted to be deactivated by another.

As development progresses other familiar game types will be added.

## How does it work?

The interface uses a single button, which changes the state of the device in game and a two line LCD screen to visually report information.

The device functions in conjunction with a two-way radio and transmits device states (game start, armed, disarmed etc) via radio to all players on the field and from the device itself.

It also includes an accelerometer as tamper detection when the device is in an armed state.

## Main contributors

- ehills
- Rukh/scanhead
- Aster/Hazesty

## Getting started

-  Install Arduino IDE.
-  Get libraries from /arduino_src/lib and move them to Arduino library environment.
-  Load the ino in /arduino_src/TheRuddyBombVX.
-  Done.

## Current hardware
- Arduino MEGA 2560
- DFRobot shield V1.0 with lcd screen
- Generic Buzzer
- Generic push-button

- The device also needs some kind of power supply - Virtually any 5V power source will suffice.

COMING SOON: HARDWARE CONFIGURATION

## UPDATE V1.2.0

- Fixed start of game tone keying to hold PTT until end of tone sequence.
- Accelerometer removed for basic version.
- Added a pitches.h for tone generation of musical melodies.