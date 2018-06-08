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
#define switchPin 51 // external button.
#define radioOutputPin 49 // radio output.
#define PTTPin 47 //PTT ground connect.

#else
//Arduino UNO settings

//Hardware
#define buzzerPin 11 //Buzzer
#define radioOutputPin 12 // radio output.
#define PTTPin 3 //PTT ground connect.

#endif

//buzzer tones
#define menuTone 262 //menu selection and unarmed bomb
#define detonatorTone 330 //on detonation
#define plantDefuseTone 659 //whenever defusal or planting is in progress
#define startTone 524 //start tone.
#define highTone 750 //high pitch for bomb-finding.

//the LCD screen has 16 squares.
#define row_len 16.0

#endif