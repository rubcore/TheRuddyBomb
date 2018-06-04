#ifndef HARDWAREPARAMS_H_
#define HARDWAREPARAMS_H_
/*
  Open Source Arduino Airsoft Project - Hardware parameters
 */

//=====HARDWARE SETTINGS=====
 
//defines the use of the external button for bomb arming/defusing.
//comment out if not using external button.
#define externalButton

//comment out the version of the device that you are NOT using.
#define ARDUINO_MEGA
//#define ARDUINO_UNO

//=====END HARDWARE SETTINGS=====
//(it is not recommended to edit past this point.)

#ifdef ARDUINO_MEGA
//Arduino MEGA settings

//Hardware
#define buzzerPin 53 //Buzzer
#define switchPin 51 // external button
#define radioOutputPin 49 // radio output.
#define radioOutputPinPair 48 //other pin.
#define PTTPin 47 //PTT ground connect.
#define PTTPinPair 46 //the other pin.

#else
//Arduino UNO settings

//Hardware
#define buzzerPin 12 //Buzzer
//#define switchPin 51 // external button
#define radioOutputPin 11 // radio output.
#define PTTPin 10 //PTT ground connect.

#endif

//buzzer tones
#define menuTone 262 //menu selection and unarmed bomb
#define detonatorTone 330 //on detonation
#define plantDefuseTone 659 //whenever defusal or planting is in progress
#define startTone 524 //start tone.

//the LCD screen has 16 squares.
#define row_len 16.0

#endif