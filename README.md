# The Ruddy Bomb!

![The ruddy bastards](http://rub.printmighty.co.nz/assets/Uploads/_resampled/xSetWidth846-webstorebanner4.png.pagespeed.ic.LG9ZfQe-v2.png)

## Getting started

l) Install Arduino IDE.
l) Get libraries from /arduino_src/lib and move them to Arduino library environment.
l) Load the ino in /arduino_src/TheRuddyBombVX.
l) Done.

## Current hardware
- Arduino MEGA 2560
- DFRobot shield V1.0 with lcd screen
- Generic Buzzer

- Will also need some kind of power supply - We use an 18650 cell with a board to 5V power.

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