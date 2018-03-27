/*
  TheRuddyBombVX - Arduino Compilable
  FOR ATMEGA2560 on ARDUINO MEGA
 */
//Function parameters

//TIME in MILLISECONDS, use u_long.
//Default settings set to:
//10s arm time
//40 second detonation time
//10s defuse
//5 minute game time
//0s penalty.

//Additional settings must be placed directly into array with the appropriate +1 to the count.

// include the library code:
#include <LiquidCrystal.h>
#include "LcdKeypad.h"

//VERSION
#define VERSION "   ver. 1.0.1   "

//Define statements
#define buzzerPin 53
#define tiltPin 51
#define PIRPin 49

//number of loops before tilt switch becomes active.
#define tiltThresholdCount 10

//delay variables for the filter for the tilt switch and PIR sensor.
#define tiltDelay 30
#define PIRDelay 30

//buzzer tones
//menu selection and unarmed bomb
#define menuTone 262
//on detonation
#define detonatorTone 330
//whenever defusal or planting is in progress
#define plantDefuseTone 659
//start tone.
#define startTone 524

//misc declerations

//the LCD screen has 16 squares.
#define row_len 16.0

//5 minutes 
#define Global_Game_Time 300000

//Bomb arm time
#define ARM_time_default 0
#define ARM_time_count 3
#define ARM_time_10s 10000
#define ARM_time_30s 30000
#define ARM_time_1m  60000

//Bomb trigger time
#define TRIG_time_default 3
#define TRIG_time_count 5
//#define TRIG_time_1m 60000
//#define TRIG_time_2m 120000
//#define TRIG_time_3m 180000
//#define TRIG_time_5m 300000
//#define TRIG_time_10m 600000

//CHANGE: shorter detonation times
#define TRIG_time_10s 10000
#define TRIG_time_20s 20000
#define TRIG_time_30s 30000
#define TRIG_time_40s 40000
#define TRIG_time_50s 50000

//Bomb defuse time
#define DEFUSE_time_default 0
#define DEFUSE_time_count 3
#define DEFUSE_time_10s 10000
#define DEFUSE_time_30s 30000
#define DEFUSE_time_1m  60000

//penalty time
#define PENALTY_time_default 1
#define PENALTY_time_count 3
#define PENALTY_time_5s 5000
#define PENALTY_time_10s 10000

//Custom characters for Progress bars
uint8_t p1[8] = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10};
uint8_t p2[8] = {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18};
uint8_t p3[8] = {0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C};
uint8_t p4[8] = {0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E};
uint8_t p5[8] = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};

//array of times
unsigned long armTimes[ARM_time_count] = {ARM_time_10s,ARM_time_30s,ARM_time_1m};
unsigned long trigTimes[TRIG_time_count] = {TRIG_time_10s,TRIG_time_20s,TRIG_time_30s,TRIG_time_40s,TRIG_time_50s};
unsigned long defuseTimes [DEFUSE_time_count] = {DEFUSE_time_10s,DEFUSE_time_30s,DEFUSE_time_1m};
unsigned long penaltyTimes [PENALTY_time_count] = {0,PENALTY_time_5s,PENALTY_time_10s};

//the current bomb setting
unsigned long arm_time_set;
unsigned long trig_time_set;
unsigned long defuse_time_set;
unsigned long penalty_time_set;

//the current time for the timers.
unsigned long current_time;
unsigned long planting_time;
unsigned long defusing_time;
//unsigned long penalty_time;

//set position of the array.
char arm_time_pos;
char trig_time_pos;
char defuse_time_pos;
char penalty_time_pos;

//device settings
boolean beep_if_unarmed = true;

//5 minutes
const unsigned long global_game_time = Global_Game_Time;
unsigned long game_start_time = 0;

//temporary array for printing.
char temp_array_16[18] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char temp_array_3[4] = {0,0,0,0};

//Game Timer Bottom Row
char global_timer_bar[18] = "G-00:00 B-00:00 ";

//option display strings
char temp_array_minute[18] = "<  00 Minutes  >";
char temp_array_second[18] = "<  00 Seconds  >";

//message display strings
char array_planting_msg[18] = "[   PLANTING   ]";
char array_defusing_msg[18] = "[   DEFUSING   ]";
char array_ticking_msg[18] = "[   PLANTED!   ]";

unsigned char tiltArray[tiltThresholdCount];
unsigned char tiltArrayPos = 0;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//Application modes
typedef enum AppModeValues {
    SELECT_BOMB_TYPE,
    WAITING_FOR_PLANT,
    BOMB_PLANTING,
    BOMB_DEFUSING,
    TIMER_RUNNING,
    APP_MENU_MODE,
    APP_PROCESS_MENU_CMD,
    APP_MENU_MODE_END
} AppMode;

typedef enum MenuMode{
  GAME_START,
  SET_ARM_TIME,
  SET_TRIG_TIME,
  SET_DEFUSE_TIME,
  SET_PENALTY_TIME
} MenuMode;

//Set the application modes
AppMode currentMode = SELECT_BOMB_TYPE;
MenuMode currentMenuMode = GAME_START;

char btn; //the button that was pressed.

//the menu setting function.
void menuSettings();
//The start of game routine.
void startGame();
//pre-start of the game routine.
void preStartGame();

//print to the screen.
void printtoScreen(char*,char*);
void printtoTop(char*);
void printtoBot(char*);
//the same but for constant strings
void printtoScreen(const char*,const char*);
void printtoTop(const char*);
void printtoBot(const char*);

//int to char array print
void Int2AsciiExt(unsigned int);

//adjust the menu values
void armTimeAdjust(char mod);
void trigTimeAdjust(char mod);
void defuseTimeAdjust(char mod);
void penTimeAdjust(char mod);

//draw the progress bar for planting and defusing.
void drawProgress(unsigned long currTime, double maxTime);

void clearTempArray(); //added 27/12/27

//end conditions.
void attackersDetonate(); //T team win
void defendersDefuse(); //CT team win
void defendersStall(); //CT team win
void endOfGameCleanup(); //cleanup the screen and go back to the menu state.

void applyPenalty(char type);
void playShortTone(int freq, int len);

//setup
void setup() {

  //setup the buzzer pin.
  pinMode(buzzerPin, OUTPUT);
  //PIR pin
  pinMode(PIRPin, INPUT);
  //tilt switch pin
  pinMode(tiltPin,INPUT);

  //for debug
  Serial.begin(9600);

  // set up the LCD's number of columns and rows.
  lcd.begin(16, 2);
  
  //create the custom characters for the progress bar.
  lcd.createChar(1, p1);
  lcd.createChar(2, p2);
  lcd.createChar(3, p3);
  lcd.createChar(4, p4);
  lcd.createChar(5, p5);

  //set the default bomb parameters.
  arm_time_set = armTimes[ARM_time_default];
  trig_time_set = trigTimes[TRIG_time_default];
  defuse_time_set = defuseTimes[DEFUSE_time_default];
  penalty_time_set = penaltyTimes[PENALTY_time_default];

  //hold the position of the pointers to the array position
  arm_time_pos = ARM_time_default;
  trig_time_pos = TRIG_time_default;
  defuse_time_pos = DEFUSE_time_default;
  penalty_time_pos = PENALTY_time_default;

  //reset the tilt sensor array
  for (int i = 0; i < tiltThresholdCount; ++i){
    tiltArray[i] = 0;
  }
  
  //counter value variables
  current_time = 0;
  planting_time = 0;
  defusing_time = 0;
  
  // Print a message to the LCD.
  printtoScreen("  TheRuddyBomb  ",VERSION);
  delay(1500);
  printtoScreen("  TheRuddyBomb  ", "   Bomb Ready   ");
  delay(1000);

  //done setting up
}

//main loop
void loop() {
  //get the button prompt.
  btn = getButton();

  if (currentMode == SELECT_BOMB_TYPE){
    menuSettings();
  }
  else if(currentMode == WAITING_FOR_PLANT){

    //booelan flag.
    boolean arming_in_progress = false;
    //temporary time.
    unsigned long temp_time = 0;
    //temporary time for the global counter
    unsigned long temp_time_global = 0;

    //get the start time.
    game_start_time = millis();
    
    //loop until we arm the bomb.
    while (true){

      //check the global timer. If at any point 5 mins is up, the game is over.
      if (millis() - game_start_time >= global_game_time){
         defendersStall();
         break;
      }

      //get the button.
      btn = getButton();

      //CHANGE HERE
      //play Unarmed bomb tone.
      //mod 10000 every 10 seconds 10% duty cycle.
      if (!arming_in_progress && beep_if_unarmed){
        if (millis() % 10000 <= 1000){
          tone(buzzerPin,menuTone);
        }
        else{
          noTone(buzzerPin);
        }
      }
      
      if (arming_in_progress == false){
        //calculate the game global timer.
        //calculate the time elapsed.
        temp_time_global = global_game_time - (millis() - game_start_time);
  
        //divide out each increment and fill in the characters.
        //fill in the characters.
        char ten_minutes = 0;
        char one_minute = 0;
        char ten_seconds = 0;
        char one_second = 0;
  
        while (temp_time_global >= 600000){
          ++ten_minutes;
          temp_time_global -= 600000;
        }
  
        while (temp_time_global >= 60000){
          ++one_minute;
          temp_time_global -= 60000;
        }
  
        while (temp_time_global >= 10000){
          ++ten_seconds;
          temp_time_global -= 10000;
        }
  
        while (temp_time_global >= 1000){
          ++one_second;
          temp_time_global -= 1000;
        }
  
        //fill in the message
        global_timer_bar[2] = ten_minutes + '0';
        global_timer_bar[3] = one_minute + '0';
        global_timer_bar[5] = ten_seconds + '0';
        global_timer_bar[6] = one_second + '0';
        
        //print to the screen
        printtoTop("[  PLANT ME!   ]");
        printtoBot(global_timer_bar);
      }
        
      //button pressed.
      if (btn == BUTTON_SELECT_PRESSED || btn == BUTTON_SELECT_LONG_PRESSED){
        //set the flag
        arming_in_progress = true;

        //blank the screen temporarily
        printtoBot("                ");

        //Now start the bomb planting timer.
        //start the timer.
        planting_time = millis();
      }
      //button released.
      else if (btn == BUTTON_SELECT_SHORT_RELEASE || btn == -59){

        //clear the tone
        noTone(buzzerPin);
    
        //deset the flag
        arming_in_progress = false;

        //reset the array with spaces.
        clearTempArray();
        
      }
      else if (arming_in_progress){
        //print the progress to the screen.

        //calculate the time remaining.
        temp_time = millis() - planting_time;

        //play 2Hz tone.
        if (temp_time % 500 >= 250){
          tone(buzzerPin,plantDefuseTone);
        }
        else{
          noTone(buzzerPin);
        }

        //print the time remaining into the array.
        //convert to seconds remaining.
        Int2AsciiExt((unsigned int)((arm_time_set - temp_time)/1000));

        //print the time to the array.
        //array_planting_msg[11] = temp_array_3[1];
        //array_planting_msg[12] = temp_array_3[0];
        
        //print to the array top line.
        printtoTop(array_planting_msg);

        //print to the array bottom line.
        drawProgress(temp_time, arm_time_set);

        printtoBot(temp_array_16);
        
        if (temp_time >= arm_time_set){

          temp_time = 0; //reset the temp time.
          
          //reset the array with spaces.
          clearTempArray();
          
          //deset the flag
          arming_in_progress = false;

          //change the state.
          currentMode = TIMER_RUNNING;

          break;
          
        }
        
      }
      
    }
    
  }
  else if(currentMode == TIMER_RUNNING){
    //3 cases: Either the detonator explodes
    //or the CT team defuses the bomb.
    //or timer (5 mins) runs out.

    //set the output
    printtoScreen("[   PLANTED!   ]","                ");

    //start the countdown.
    //check millis.
    unsigned long countdown_time = millis();
   
    //we will wait until the button has been released to start the next section.
    while (!(btn == BUTTON_SELECT_SHORT_RELEASE || btn == -59)) btn = getButton();

    //setup detonate variables
    unsigned long temp_time = 0;
    unsigned long defuse_time = 0;
    unsigned long time_remaining = 0;
    unsigned long temp_time_global = 0;
    boolean disarming_in_progress = false;
    
    //loop forever
    while (true){

      //check the global timer. If at any point 5 mins is up, the game is over.
      if (millis() - game_start_time >= global_game_time){
         defendersStall();
         break;
      }

      //get the button
      btn = getButton();

      //setup the time remaining on the detonator.
      if (btn == BUTTON_SELECT_PRESSED || btn == BUTTON_SELECT_LONG_PRESSED){
        //set the flag
        disarming_in_progress = true;

        //Now start the bomb planting timer.
        //start the timer.
        defuse_time = millis();
      }
      //button released.
      else if (btn == BUTTON_SELECT_SHORT_RELEASE || btn == -59){

        //clear the tone
        noTone(buzzerPin);

        //deset the flag
        disarming_in_progress = false;

        //reset the array with spaces.
        clearTempArray();
        
      }
      else if (disarming_in_progress){
        //print the progress to the screen.

        //calculate the time remaining.
        temp_time = millis() - defuse_time;
        
        //play 2Hz tone.
        if (temp_time % 500 >= 250){
          tone(buzzerPin,plantDefuseTone);
        }
        else{
          noTone(buzzerPin);
        }

        //print the time remaining into the array.
        //convert to seconds remaining.
        Int2AsciiExt((unsigned int)((defuse_time_set - temp_time)/1000));

        //array_defusing_msg[11] = temp_array_3[1];
        //array_defusing_msg[12] = temp_array_3[0];
        
        //print to the array top line.
        printtoTop(array_defusing_msg);
        
        //print to the array bottom line.
        //print the time to the array.
        drawProgress(temp_time, defuse_time_set);

        //BOMB DEFUSED
        printtoBot(temp_array_16);

        //if the timer exceeds the defuse time.
        if (temp_time >= defuse_time_set){
          //reset the temp time
          temp_time = 0;
          //bomb is defused
          defendersDefuse();
          //exit out of while loop.
          break;
        }
        
      }
      else{ //just draw the countdown timer.

        //fill in the countdown.

        //calculate the time remaining.
        time_remaining = trig_time_set - (millis() - countdown_time);

        //divide out each increment and fill in the characters.
        //fill in the characters.
        //char ten_minutes = 0;
        //char one_minute = 0;
        char ten_seconds = 0;
        char one_second = 0;

        /*
        while (time_remaining >= 600000){
          ++ten_minutes;
          time_remaining -= 600000;
        }

        while (time_remaining >= 60000){
          ++one_minute;
          time_remaining -= 60000;
        }
        */

        while (time_remaining >= 10000){
          ++ten_seconds;
          time_remaining -= 10000;
        }

        while (time_remaining >= 1000){
          ++one_second;
          time_remaining -= 1000;
        }

        //fill in the message
        //global_timer_bar[10] = ten_minutes + '0';
        //global_timer_bar[11] = one_minute + '0';
        global_timer_bar[13] = ten_seconds + '0';
        global_timer_bar[14] = one_second + '0';

        //calculate the game global timer.
        //calculate the time elapsed.
        temp_time_global = global_game_time - (millis() - game_start_time);
  
        //divide out each increment and fill in the characters.
        //fill in the characters.
        char ten_minutes = 0;
        char one_minute = 0;
        ten_seconds = 0;
        one_second = 0;
  
        while (temp_time_global >= 600000){
          ++ten_minutes;
          temp_time_global -= 600000;
        }
  
        while (temp_time_global >= 60000){
          ++one_minute;
          temp_time_global -= 60000;
        }
  
        while (temp_time_global >= 10000){
          ++ten_seconds;
          temp_time_global -= 10000;
        }
  
        while (temp_time_global >= 1000){
          ++one_second;
          temp_time_global -= 1000;
        }
  
        //fill in the message
        global_timer_bar[2] = ten_minutes + '0';
        global_timer_bar[3] = one_minute + '0';
        global_timer_bar[5] = ten_seconds + '0';
        global_timer_bar[6] = one_second + '0';

        //print the array to the screen to indicate time remaining
        //set the output array on top
        printtoTop("[   PLANTED!   ]");
        printtoBot(global_timer_bar);

        //1Hz detonator tone
        if (time_remaining >= 500) tone(buzzerPin,detonatorTone);
        else noTone(buzzerPin);
        
      }
      //BOMB DETONATES
      if ((millis() - countdown_time) >= trig_time_set){
        
        temp_time = 0; //reset the temp time.

        //bomb detonates
        attackersDetonate();

        //exit out of loop
        break;
      }
      
    }
  }
}

//On the start screen.
void preStartGame(){
  printtoTop("[PRESS TO START]");

  //set the current game parameters on the display screen
  //set game timer.
  //fill in the characters.
  char ten_minutes = 0;
  char one_minute = 0;
  char ten_seconds = 0;
  char one_second = 0;

  unsigned long temp_time = global_game_time;

  while (temp_time >= 600000){
    ++ten_minutes;
    temp_time -= 600000;
  }

  while (temp_time >= 60000){
    ++one_minute;
    temp_time -= 60000;
  }

  while (temp_time >= 10000){
    ++ten_seconds;
    temp_time -= 10000;
  }

  while (temp_time >= 1000){
    ++one_second;
    temp_time -= 1000;
  }

  //fill in the message
  global_timer_bar[2] = ten_minutes + '0';
  global_timer_bar[3] = one_minute + '0';
  global_timer_bar[5] = ten_seconds + '0';
  global_timer_bar[6] = one_second + '0';

  //set bomb timer
  ten_seconds = 0;
  one_second = 0;

  temp_time = trig_time_set;

  while (temp_time >= 10000){
    ++ten_seconds;
    temp_time -= 10000;
  }

  while (temp_time >= 1000){
    ++one_second;
    temp_time -= 1000;
  }

  global_timer_bar[13] = ten_seconds + '0';
  global_timer_bar[14] = one_second + '0';

  printtoBot(global_timer_bar);

}

//start the game
//This function is the start process.
void startGame(){

  printtoBot("                ");

  //Delay 3 seconds with sound feedback
  printtoTop("[STARTING IN: 3]");
  playShortTone(menuTone,1000);
  delay(750);
  printtoTop("[STARTING IN: 2]");
  playShortTone(menuTone,1000);
  delay(750);
  printtoTop("[STARTING IN: 1]");
  playShortTone(menuTone,1000);
  delay(750);
  printtoTop("[  GAME START  ]");
  playShortTone(startTone,1000);
}

//set bomb parameters.
void menuSettings(){

  //print the current settings.
  preStartGame();
  
  //while true. Exit when the bomb is ready to be played.
  while (true){

    btn = getButton();

    if (btn == BUTTON_DOWN_PRESSED){

      playShortTone(menuTone,50);
      
      //loop the settings
      if (currentMenuMode == GAME_START){
        currentMenuMode = SET_ARM_TIME;
        printtoTop("Set Arm Time    ");
        armTimeAdjust(0);
      }
      else if (currentMenuMode == SET_ARM_TIME){
        currentMenuMode = SET_TRIG_TIME;
        printtoTop("Set Detonator   ");
        trigTimeAdjust(0);
      }
      else if (currentMenuMode == SET_TRIG_TIME){
        currentMenuMode = SET_DEFUSE_TIME;
        printtoTop("Set Defuse Time ");
        defuseTimeAdjust(0);
      }
      else if (currentMenuMode == SET_DEFUSE_TIME){
        currentMenuMode = SET_PENALTY_TIME;
        printtoTop("Set Pen Time    ");
        penTimeAdjust(0);
      }
      else if (currentMenuMode == SET_PENALTY_TIME){
        currentMenuMode = GAME_START;
        preStartGame();
      }
  
    }
    else if (btn == BUTTON_UP_PRESSED){
      
      playShortTone(menuTone,50);
      
      //loop the settings
      if (currentMenuMode == SET_TRIG_TIME){
        currentMenuMode = SET_ARM_TIME;
        printtoTop("Set Arm Time    ");
        armTimeAdjust(0);
      }
      else if (currentMenuMode == SET_DEFUSE_TIME){
        currentMenuMode = SET_TRIG_TIME;
        printtoTop("Set Detonator   ");
        trigTimeAdjust(0);
      }
      else if (currentMenuMode == SET_PENALTY_TIME){
        currentMenuMode = SET_DEFUSE_TIME;
        printtoTop("Set Defuse Time ");
        defuseTimeAdjust(0);
      }
      else if (currentMenuMode == GAME_START){
        currentMenuMode = SET_PENALTY_TIME;
        printtoTop("Set Pen Time    ");
        penTimeAdjust(0);
      }
      else if (currentMenuMode == SET_ARM_TIME){
        currentMenuMode = GAME_START;
        preStartGame();
      }
      
    }
    else if (btn == BUTTON_LEFT_PRESSED){
      
      playShortTone(menuTone,50);
      
      if (currentMenuMode == SET_ARM_TIME){
        armTimeAdjust(-1);
      }
      else if (currentMenuMode == SET_TRIG_TIME){
        trigTimeAdjust(-1);
      }
      else if (currentMenuMode == SET_DEFUSE_TIME){
        defuseTimeAdjust(-1);
      }
      else if (currentMenuMode == SET_PENALTY_TIME){
        penTimeAdjust(-1);
      }
      
    }
    else if (btn == BUTTON_RIGHT_PRESSED){

      playShortTone(menuTone,50);
            
      if (currentMenuMode == SET_ARM_TIME){
        armTimeAdjust(1);
      }
      else if (currentMenuMode == SET_TRIG_TIME){
        trigTimeAdjust(1);
      }
      else if (currentMenuMode == SET_DEFUSE_TIME){
        defuseTimeAdjust(1);
      }
      else if (currentMenuMode == SET_PENALTY_TIME){
        penTimeAdjust(1);
      }
    }
    else if (btn == BUTTON_SELECT_PRESSED){

      //we start the bomb.
      //Game starts here.
      if (currentMenuMode == GAME_START){
        
        //set the mode to bomb waiting to plant.
        currentMode = WAITING_FOR_PLANT;
        //exit back to main loop.

        //start the game
        startGame();
        
        return;
      }
      
    }
    
  }
  
}

//print text to screen on two rows.
void printtoScreen(char * top_row, char* bot_row){
  //print the stuff.
  lcd.setCursor(0, 0);
  lcd.print(top_row);
  lcd.setCursor(0, 1);
  lcd.print(bot_row);
  
}

void printtoTop(char* top_row){
  //print the stuff
  lcd.setCursor(0, 0);
  lcd.print(top_row);
}

void printtoBot(char* bot_row){
  //print the stuff
  lcd.setCursor(0, 1);
  lcd.print(bot_row);
}

//print text to screen on two rows.
void printtoScreen(const char * top_row,const char* bot_row){
  //print the stuff.
  lcd.setCursor(0, 0);
  lcd.print(top_row);
  lcd.setCursor(0, 1);
  lcd.print(bot_row);
}

void printtoTop(const char* top_row){
  //print the stuff
  lcd.setCursor(0, 0);
  lcd.print(top_row);
}

void printtoBot(const char* bot_row){
  //print the stuff
  lcd.setCursor(0, 1);
  lcd.print(bot_row);
}

void armTimeAdjust(char mod){
  
  //already at the bounds of the array
  if ((arm_time_pos == 0 && mod == -1 )||(arm_time_pos == (ARM_time_count - 1) && mod == 1)){
    //do nothing to prevent increment past bounds.
    return;
  }

  if (mod == 1) //increment
  {
    ++arm_time_pos;
    arm_time_set = armTimes[arm_time_pos];
  }
  else if (mod == -1) //decrement
  {
    --arm_time_pos;
    arm_time_set = armTimes[arm_time_pos];
  }

  //get the number in milliseconds to seconds, and print.
  Int2AsciiExt((unsigned int)(arm_time_set/1000));

  //place the data into the array.
  temp_array_second[3] = temp_array_3[1];
  temp_array_second[4] = temp_array_3[0];

  //now print the values to the screen.
  //change the chevron direction.
  if (arm_time_pos == 0){
    temp_array_second[0] = ' ';
    temp_array_second[15] = '>';
  }
  else if (arm_time_pos == (ARM_time_count - 1)){
    
    temp_array_second[0] = '<';
    temp_array_second[15] = ' ';
  }
  else{
    temp_array_second[0] = '<';
    temp_array_second[15] = '>';
  }

  printtoBot(temp_array_second);
}

void trigTimeAdjust(char mod){
    //already at the bounds of the array
  if ((trig_time_pos == 0 && mod == -1 )||(trig_time_pos == (TRIG_time_count - 1) && mod == 1)){
    //do nothing to prevent increment past bounds.
    return;
  }

  if (mod == 1) //increment
  {
    ++trig_time_pos;
    trig_time_set = trigTimes[trig_time_pos];
  }
  else if (mod == -1) //decrement
  {
    --trig_time_pos;
    trig_time_set = trigTimes[trig_time_pos];
  }

  //get the number in milliseconds to seconds, and print.
  Int2AsciiExt((unsigned int)(trig_time_set/1000));

  //place the data into the array.
  temp_array_second[3] = temp_array_3[1];
  temp_array_second[4] = temp_array_3[0];

  //now print the values to the screen.
  //change the chevron direction.
  if (trig_time_pos == 0){
    temp_array_second[0] = ' ';
    temp_array_second[15] = '>';
  }
  else if (trig_time_pos == (TRIG_time_count - 1)){
    
    temp_array_second[0] = '<';
    temp_array_second[15] = ' ';
  }
  else{
    temp_array_second[0] = '<';
    temp_array_second[15] = '>';
  }

  printtoBot(temp_array_second);
}

void defuseTimeAdjust(char mod){
  
  //already at the bounds of the array
  if ((defuse_time_pos == 0 && mod == -1 )||(defuse_time_pos == (DEFUSE_time_count - 1) && mod == 1)){
    //do nothing to prevent increment past bounds.
    return;
  }

  if (mod == 1) //increment
  {
    ++defuse_time_pos;
    defuse_time_set = defuseTimes[defuse_time_pos];
  }
  else if (mod == -1) //decrement
  {
    --defuse_time_pos;
    defuse_time_set = defuseTimes[defuse_time_pos];
  }

  //get the number in milliseconds to seconds, and print.
  Int2AsciiExt((unsigned int)(defuse_time_set/1000));

  //place the data into the array.
  temp_array_second[3] = temp_array_3[1];
  temp_array_second[4] = temp_array_3[0];

  //now print the values to the screen.
  //change the chevron direction.
  if (defuse_time_pos == 0){
    temp_array_second[0] = ' ';
    temp_array_second[15] = '>';
  }
  else if (defuse_time_pos == (DEFUSE_time_count - 1)){
    
    temp_array_second[0] = '<';
    temp_array_second[15] = ' ';
  }
  else{
    temp_array_second[0] = '<';
    temp_array_second[15] = '>';
  }

  printtoBot(temp_array_second);
}

void penTimeAdjust(char mod){
  
  //already at the bounds of the array
  if ((penalty_time_pos == 0 && mod == -1 )||(penalty_time_pos == (PENALTY_time_count - 1) && mod == 1)){
    //do nothing to prevent increment past bounds.
    return;
  }

  //change the setting

  if (mod == 1) //increment
  {
    ++penalty_time_pos;
    penalty_time_set = penaltyTimes[penalty_time_pos];
  }
  else if (mod == -1) //decrement
  {
    --penalty_time_pos;
    penalty_time_set = penaltyTimes[penalty_time_pos];
  }

  //get the number in milliseconds to seconds, and print.
  Int2AsciiExt((unsigned int)(penalty_time_set/1000));

  //place the data into the array.
  temp_array_second[3] = temp_array_3[1];
  temp_array_second[4] = temp_array_3[0];

  //now print the values to the screen.
  //change the chevron direction.
  if (penalty_time_pos == 0){
    temp_array_second[0] = ' ';
    temp_array_second[15] = '>';
  }
  else if (penalty_time_pos == (PENALTY_time_count - 1)){
    
    temp_array_second[0] = '<';
    temp_array_second[15] = ' ';
  }
  else{
    temp_array_second[0] = '<';
    temp_array_second[15] = '>';
  }

  //print the setting output.
  printtoBot(temp_array_second);
}

//Int number to ascii string
//assumes char* return is 3 bytes.
void Int2AsciiExt(unsigned int input){  

  //Set numbers
  unsigned char hundreds = 0, tens = 0, units = 0; //hold val for each area
  tens = input / 10; //set tens
  units = input % 10; //set units

  //Get hundreds
  // modulo the tens.
  while (tens > 9)
  {
    hundreds++;
    tens -= 10;
  }

  //Set the array.
  temp_array_3[0] = units + '0';
  temp_array_3[1] = tens + '0';
  temp_array_3[2] = hundreds + '0';
}

//draw a progress bar
void drawProgress(unsigned long currTime, unsigned long maxTime) {
    
    //calculate the percentage time.
    double percent = (double)((currTime * 100.0)/maxTime );
    
    //calculate the number of columns to fill
    double colsToFill = row_len / 100 * percent;

    //loop variables
    char existingDrawn = 0;
    char character = 0;
    int loop_v = 0;
    
    //fill in full bar up to the position of the partial bar.
    if (colsToFill >= 1) {
        //loop over all columns until we are done.
        for (loop_v = 0; loop_v <= colsToFill; ++loop_v) {
            //draw full.
            temp_array_16[loop_v] = (char)(5);
            existingDrawn = loop_v;
        }
        
        //decrement until there are no more.
        colsToFill = colsToFill - existingDrawn;
    }
    //get the partial bar calculated
    character = colsToFill * 5;
    
    //print the last bar.
    switch (character) {

        case 0:
            break;

        case 1:
            temp_array_16[loop_v] = ((char) 1);
            break;

        case 2:
            temp_array_16[loop_v] = ((char) 2);
            break;

        case 3:
            temp_array_16[loop_v] = ((char) 3);
            break;

        case 4:
            temp_array_16[loop_v] = ((char) 4);
            break;

        default:
            break;
    }
}

//bomb is defused
void defendersDefuse(){

  //print notification
  printtoTop("[   DEFUSED!   ]");
  printtoBot(global_timer_bar);
  
  //descending tones.
  for (int i = 500; i >= 100; --i){
    tone(buzzerPin,i);
    delay(5);
  }

  //cleanup stuff
  endOfGameCleanup();
}

//bomb is detonated
void attackersDetonate(){
  
  //print notification.
  printtoTop("[  DETONATED!  ]");
  printtoBot(global_timer_bar);

  //play detonation tone.
  tone(buzzerPin, detonatorTone);
  
  //cleanup stuff
  endOfGameCleanup();
}

//the global timer runs out.
void defendersStall(){
  
  //print notification.
  printtoTop("[  ROUND END!  ]");
  printtoBot(global_timer_bar);

  //cleanup stuff
  endOfGameCleanup();
}

//cleanup the end of the game
void endOfGameCleanup(){
  
  // go back to the bomb type.
  currentMode = SELECT_BOMB_TYPE;

  //cleanup the progress bar
  clearTempArray();

  //reset the set times.
  //arm_time_set = armTimes[arm_time_pos];
  //defuse_time_set = defuseTimes[defuse_time_pos];
  
  //we will wait until the button has been released to restart.
  while (!(btn == BUTTON_RIGHT_PRESSED)) btn = getButton();
  
  //Let Menu stuff be visible when we go back to the bomb set state.
  currentMenuMode = GAME_START;

  noTone(buzzerPin);
}

//clear the temporary array.
void clearTempArray(){
  //set the array to just spaces
 for (char i = 0; i < 16; ++i){
   temp_array_16[i] = ' ';
  }
}

//apply the penalty
void applyPenalty(char type){
  if (type == 1){ //apply to attackers
    arm_time_set += penalty_time_set;
    //can also decrease the detonator
  }
  else if (type == -1){ //apply to defenders
    defuse_time_set += penalty_time_set;
    //can also increase the detonator
  }
}

//play a short tone of freq
void playShortTone(int freq, int len){
  
  tone(buzzerPin,freq); //turn on buzzer
  delay(len); //do a delay
  noTone(buzzerPin); //turn off buzzer
}
