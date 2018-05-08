#ifndef HARDWAREPARAMS_H_
#define HARDWAREPARAMS_H_
/*
  Open Source Arduino Airsoft Project - Hardware parameters
 */

//defines the use of the external button for bomb arming/defusing.
//comment out if not using external button.
#define externalButton

//Hardware
#define buzzerPin 53 //Buzzer
#define switchPin 51 // external button
#define radioOutputPin 49 // radio output.
#define radioOutputPinPair 48 //other pin.

//ADC Accelerometer
#define Z_axis A1
#define Y_axis A2
#define X_axis A3

//buzzer tones
#define menuTone 262 //menu selection and unarmed bomb
#define detonatorTone 330 //on detonation
#define plantDefuseTone 659 //whenever defusal or planting is in progress
#define startTone 524 //start tone.

//the LCD screen has 16 squares.
#define row_len 16.0

#define accelerometer_enable
//high values for low sensitivity
//low values for high sensitivity
#define accel_sensitivity 10
#define accel_max_variance 50

#endif