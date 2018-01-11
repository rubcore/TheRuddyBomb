/*
  TheRuddyBombVX - Arduino Compilable - Short version 0.1.0
  FOR ATMEGA2560 on ARDUINO MEGA
  Features:
  - LCD + Buttons
  - Buzzer

  Features TODO:
  - Tilt Switch
  - LDR
 */
//Function parameters

//TIME in MILLISECONDS, use u_long.
//Default settings set to 30s arm time, 5 minute detonation time, 30s defuse, 0s penalty.
//Additional settings must be placed directly into array with the appropriate +1 to the count.

// include the library code:
#include <LiquidCrystal.h>
#include "LcdKeypad.h"

//Buzzer define statements
#define buzzerPin 51
#define menuTone 262
#define detonatorTone 330
#define plantDefuseTone 659
#define startTone 524

//misc declerations

//the LCD screen has 16 squares.
#define row_len 16.0

//Bomb arm time
#define ARM_time_default 1
#define ARM_time_count 3
#define ARM_time_10s 10000
#define ARM_time_30s 30000
#define ARM_time_1m  60000

//Bomb trigger time
#define TRIG_time_default 3
#define TRIG_time_count 5
#define TRIG_time_1m 60000
#define TRIG_time_2m 120000
#define TRIG_time_3m 180000
#define TRIG_time_5m 300000
#define TRIG_time_10m 600000

//Bomb defuse time
#define DEFUSE_time_default 1
#define DEFUSE_time_count 3
#define DEFUSE_time_10s 10000
#define DEFUSE_time_30s 30000
#define DEFUSE_time_1m  60000

//penalty time
#define PENALTY_time_default 0
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
unsigned long trigTimes[TRIG_time_count] = {TRIG_time_1m,TRIG_time_2m,TRIG_time_3m,TRIG_time_5m,TRIG_time_10m};
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

//temporary array for printing.
char temp_array_16[18] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char temp_array_3[4] = {0,0,0,0};

//option display strings
char temp_array_minute[18] = "<  00 Minutes  >";
char temp_array_second[18] = "<  00 Seconds  >";

//message display strings
char array_planting_msg[18] = "[Planting: 00s ]";
char array_defusing_msg[18] = "[Defusing: 00s ]";
char array_ticking_msg[18] = "[Planted: 00:00]";

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
  SET_PENALTY_TME
} MenuMode;

//Set the application modes
AppMode currentMode = SELECT_BOMB_TYPE;
MenuMode currentMenuMode = GAME_START;

char btn; //the button that was pressed.

//the menu setting function.
void menuSettings();

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
void attackersDetonate();
void defendersDefuse();
void endOfGameCleanup(); //cleanup the screen and go back to the menu state.

void applyPenalty(char type);
void playShortTone(int freq, int len);

//setup
void setup() {

  //setup the buzzer pin.
  pinMode(buzzerPin, OUTPUT);

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

  //counter value variables
  current_time = 0;
  planting_time = 0;
  defusing_time = 0;
  
  // Print a message to the LCD.
  //printtoScreen("  TheRuddyBomb  ","  ver. 7.7.7.X  ");
  //delay(1500);
  printtoScreen("  TheRuddyBomb  ", "   Bomb Ready   ");

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
    
    //Write to the screen.
    printtoScreen("Plant the bomb! ","                ");

    //loop until we arm the bomb.
    while (true){

      //get the button.
      btn = getButton();

      //button pressed.
      if (btn == BUTTON_SELECT_PRESSED || btn == BUTTON_SELECT_LONG_PRESSED){
        //set the flag
        arming_in_progress = true;

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
        
        //Reset the screen.
        printtoScreen("Plant the bomb! ","                ");
        
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
        array_planting_msg[11] = temp_array_3[1];
        array_planting_msg[12] = temp_array_3[0];
        
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
    //2 cases: Either the detonator explodes
    //or the CT team defuses the bomb.

    //set the output
    printtoScreen("[   Planted!   ]","                ");

    //start the countdown.
    //check millis.
    unsigned long countdown_time = millis();
   
    //we will wait until the button has been released to start the next section.
    while (!(btn == BUTTON_SELECT_SHORT_RELEASE || btn == -59)) btn = getButton();

    //setup detonate variables
    unsigned long temp_time = 0;
    unsigned long defuse_time = 0;
    unsigned long time_remaining = 0;
    boolean disarming_in_progress = false;
    
    //loop forever
    while (true){

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

        array_defusing_msg[11] = temp_array_3[1];
        array_defusing_msg[12] = temp_array_3[0];
        
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
        char ten_minutes = 0;
        char one_minute = 0;
        char ten_seconds = 0;
        char one_second = 0;

        while (time_remaining >= 600000){
          ++ten_minutes;
          time_remaining -= 600000;
        }

        while (time_remaining >= 60000){
          ++one_minute;
          time_remaining -= 60000;
        }

        while (time_remaining >= 10000){
          ++ten_seconds;
          time_remaining -= 10000;
        }

        while (time_remaining >= 1000){
          ++one_second;
          time_remaining -= 1000;
        }

        //fill in the message
        array_ticking_msg[10] = ten_minutes + '0';
        array_ticking_msg[11] = one_minute + '0';
        array_ticking_msg[13] = ten_seconds + '0';
        array_ticking_msg[14] = one_second + '0';

        //print the array to the screen to indicate time remaining
        printtoTop(array_ticking_msg);
        printtoBot("                ");

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

//set bomb parameters.
void menuSettings(){
  
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
        currentMenuMode = SET_PENALTY_TME;
        printtoTop("Set Pen Time    ");
        penTimeAdjust(0);
      }
      else if (currentMenuMode == SET_PENALTY_TME){
        currentMenuMode = GAME_START;
        printtoScreen("  TheRuddyBomb  ", "   Bomb Ready   ");
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
      else if (currentMenuMode == SET_PENALTY_TME){
        currentMenuMode = SET_DEFUSE_TIME;
        printtoTop("Set Defuse Time ");
        defuseTimeAdjust(0);
      }
      else if (currentMenuMode == GAME_START){
        currentMenuMode = SET_PENALTY_TME;
        printtoTop("Set Pen Time    ");
        penTimeAdjust(0);
      }
      else if (currentMenuMode == SET_ARM_TIME){
        currentMenuMode = GAME_START;
        printtoScreen("  TheRuddyBomb  ", "   Bomb Ready   ");
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
      else if (currentMenuMode == SET_PENALTY_TME){
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
      else if (currentMenuMode == SET_PENALTY_TME){
        penTimeAdjust(1);
      }
    }
    else if (btn == BUTTON_SELECT_PRESSED){

      //we start the bomb.
      if (currentMenuMode == GAME_START){
        
        playShortTone(startTone,1000);
      
        //set the mode to bomb waiting to plant.
        currentMode = WAITING_FOR_PLANT;
        //exit back to main loop.
        
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
  Int2AsciiExt((unsigned int)(trig_time_set/60000));

  //place the data into the array.
  temp_array_minute[3] = temp_array_3[1];
  temp_array_minute[4] = temp_array_3[0];

  //now print the values to the screen.
  //change the chevron direction.
  if (trig_time_pos == 0){
    temp_array_minute[0] = ' ';
    temp_array_minute[15] = '>';
  }
  else if (trig_time_pos == (TRIG_time_count - 1)){
    
    temp_array_minute[0] = '<';
    temp_array_minute[15] = ' ';
  }
  else{
    temp_array_minute[0] = '<';
    temp_array_minute[15] = '>';
  }

  printtoBot(temp_array_minute);
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
            //Serial.println("ERROR - Unknown character to print");
            break;
    }
}

//bomb is defused
void defendersDefuse(){

  //print notification
  printtoScreen("Game Over Man   ", "CT Defuse Win   ");

  //descending tones.
  for (int i = 500; i >= 100; --i){
    tone(buzzerPin,i);
    delay(5);
  }
  delay(3000);
  noTone(buzzerPin);
  
  //cleanup stuff
  endOfGameCleanup();
}

//bomb is detonated
void attackersDetonate(){
  
  //print notification.
  printtoScreen("Game Over Man   ","Terrorists Win  ");

  //ascending tones.
  for (int i = 100; i < 1000; ++i){
    tone(buzzerPin,i);
    delay(1);
  }
  delay(3000);
  noTone(buzzerPin);
  
  //cleanup stuff
  endOfGameCleanup();
}

//cleanup the end of the game
void endOfGameCleanup(){

  //clear out the buzzer
  noTone(buzzerPin);
  
  // go back to the bomb type.
  currentMode = SELECT_BOMB_TYPE;

  //cleanup the progress bar
  clearTempArray();

  //reset the set times.
  arm_time_set = armTimes[arm_time_pos];
  defuse_time_set = defuseTimes[defuse_time_pos];
  
  //we will wait until the button has been released to restart.
  while (!(btn == BUTTON_SELECT_SHORT_RELEASE || btn == -59)) btn = getButton();
  
  //Let Menu stuff be visible when we go back to the bomb set state.
  currentMenuMode = GAME_START;
  printtoScreen("  TheRuddyBomb  ", "   Bomb Ready   ");
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

