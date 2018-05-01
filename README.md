# The Ruddy Bomb!

![The ruddy bastards](http://rub.printmighty.co.nz/assets/Uploads/_resampled/xSetWidth846-webstorebanner4.png.pagespeed.ic.LG9ZfQe-v2.png)

## The goal of this project.

To create a simple and reliable Airsoft game device that enhances play by introducing electronically automated objectives and measured team performances.  

Assembled with an Arduino board and other off the shelf components, it is designed to be built by anyone with limited electronics knowledge. 

The interface is as simple and intuitive as possible, lowering the learning curve and creating a device that is easy to use in game.

In an attempt to encourage wide adoption we have put focus on keeping the production cost low, making the code open source and having the assembly process as simple as possible.

## What does it do?

It simulates a well known game type called Search and Destroy. The device is planted on the field by one team, and then is attempted to be deactivated by another.

As development progresses other familiar game types will be added.

## How does it work?

The interface uses a single button, which changes the state of the device in game and a two line LCD screen to visually report information.

The device functions in conjunction with a two-way radio and transmits device states (game start, armed, disarmed etc) via radio to all players on the field and from the device itself.

## Getting started

-  Install Arduino IDE.
-  Get libraries from /arduino_src/lib and move them to Arduino library environment.
-  Load the ino in /arduino_src/TheRuddyBombVX.
-  Done.

## Current hardware
- Arduino MEGA 2560
- DFRobot shield V1.0 with lcd screen
- Generic Buzzer
- ADC accelerometer
- Generic push-button

- The device also needs some kind of power supply - Virtually any 5V power source will suffice.

## Changelog V1.0.4

- Added external button support, toggle on/off by commenting out compiler directive.
- Added foul state to indicate that a device has been moved incorrectly/ Previous implementation featured the attackers detonate state which was ambiguous.
- Added function in menu for a radio check by playing a 1Hz 50% duty cycle tone to check radio.
- Added positions for radio sound output. (Not yet implemented.)

## Changelog V1.0.3

- added accelerometer support for ADC based accelerometer
- Completely removed penalty system for bomb moving... Accelerometer now detects motion and automatically detonates on movement.
- Game time can now be changed in 1 minute increments from 2 to 10 minutes global game time. Default 5 minutes.
- Further improved code size.

## Changelog V1.0.2

- Implemented an accelerated beeping process, adding to the tension at round-ending times.
- Implemented improved gameplay device states.
- Added some additional code optimization.
- Cleaned up code in some particularly verbose areas.

## Changelog V1.0.1

- Added Version. Current version is 1.0.1
- Added 3 second timer with 3-beep sequence to indicate start of round.
- Added 5 minute Global Timer. Device can now play CS:GO standard SnD.
- Global timer overwrites all other timers.
- Added timer displays for both global timer and current det timer when device is armed.
- Added a small tone every 10 seconds to allow device to be located when carrying player is hit.
- Added continuous tone on game end.
- Ending screens shows stopped timers and game state before moving on.
- Default settings changed to 10 sec arm/disarm, 40 sec trigger.
- Tilt switch temporarily removed: Use honesty system with respect to device movement.
- Penalty system removed: Option still in menu but has no function.
- Use Right button on ending screen to prevent button fall-through. Hack used -> better solution later.
