/*
  TheRuddyBombVX - ATMEGA2560 on ARDUINO MEGA
  Open Source Arduino Airsoft Project.

  Capture and control Code
 */
 
#include <LiquidCrystal.h> //LCD screen
#include "LcdKeypad.h" //LCD keypad
#include "HardwareParams.h" //hardware settings
#include "GameParams.h" //gameplay settings

#define VERSION "   ver. 0.0.1   " //VERSION

#define TeamSwitchPin 44 //for MEGA first.

//Custom characters for Progress bars
uint8_t p1[8] = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10};
uint8_t p2[8] = {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18};
uint8_t p3[8] = {0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C};
uint8_t p4[8] = {0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E};
uint8_t p5[8] = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};

//array of times
unsigned long armTimes[ARM_time_count] = {ARM_time_10s,ARM_time_20s,ARM_time_30s,ARM_time_40s, ARM_time_50s};
unsigned long trigTimes[TRIG_time_count] = {TRIG_time_20s,TRIG_time_30s,TRIG_time_40s,TRIG_time_50s,TRIG_time_60s};
unsigned long defuseTimes[DEFUSE_time_count] = {DEFUSE_time_10s,DEFUSE_time_20s,DEFUSE_time_30s,DEFUSE_time_40s,DEFUSE_time_50s};
unsigned long gameTimes[GAME_time_count] = {GAME_time_2m,GAME_time_3m,GAME_time_4m,GAME_time_5m,GAME_time_6m,GAME_time_7m,GAME_time_8m,GAME_time_9m,GAME_time_10m};

//for capture and control
const unsigned long winTime = 300000; //5 minutes
unsigned long bluePoints = 300000;
unsigned long redPoints = 300000;

//the current bomb setting
unsigned long arm_time_set;
unsigned long trig_time_set;
unsigned long defuse_time_set;
unsigned long game_time_set;

//the current time for the timers.
unsigned long current_time;
unsigned long planting_time;

//set position of the array.
char arm_time_pos;
char trig_time_pos;
char defuse_time_pos;
char game_time_pos;

unsigned long game_start_time = 0; //start time: set as global variable

//temporary array for printing.
char temp_array_16[18] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //for progress bar
char temp_array_3[4] = {0,0,0,0}; //for numbers

char global_timer_bar[18] = "G-00:00 B-00:00 ";
char temp_array_minute[18] = "<  00 Minutes  >";
char temp_array_second[18] = "<  00 Seconds  >";

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); //init the LCD object.

typedef enum AppModeValues {
    SELECT_BOMB_TYPE, WAITING_FOR_PLANT, BOMB_PLANTING, BOMB_DEFUSING, TIMER_RUNNING, APP_MENU_MODE, APP_PROCESS_MENU_CMD, APP_MENU_MODE_END
} AppMode;

typedef enum CCModeValues {
  MENU, DEVICE_NEUTRAL, RED_TEAM_POS, GREEN_TEAM_POS, COUNTING_DOWN
} CCMode; //capture and control.

typedef enum MenuMode{
  GAME_START, SET_ARM_TIME, SET_TRIG_TIME, SET_DEFUSE_TIME, SET_GAME_TIME, SET_RADIO_CHECK
} MenuMode;

//AppMode currentMode = SELECT_BOMB_TYPE;
CCMode currentMode = MENU;
MenuMode currentMenuMode = GAME_START;

char btn; //the button that was pressed.

void menuSettings(); //do menu things

void startGame(); //The start of game routine.
void preStartGame(); //pre-start of the game routine: Output game settings

void printtoScreen(char*,char*);
void printtoTop(char*);
void printtoBot(char*);

void printtoScreen(const char*,const char*);
void printtoTop(const char*);
void printtoBot(const char*);

void Int2AsciiExt(unsigned int); //int to char array print

//adjust the menu values
void armTimeAdjust(char mod);
void trigTimeAdjust(char mod);
void defuseTimeAdjust(char mod);
void gameTimeAdjust(char mod);

//timer output function.
void timerDisplay(unsigned long input); //fn outputs to timerArray4
char timerArray4[4] = {'0','0','0','0'};

void drawProgress(unsigned long currTime, double maxTime); //draw the progress bar

void clearTempArray();

//win conditions.
void attackersDetonate(); //T team win: Successful Detonation
void foulDetonate(); //T team win: bomb was moved by CT.
void defendersDefuse(unsigned long BombTime); //CT team win: Successful Defuse
void defendersStall(); //CT team win: Timeout
void endOfGameCleanup(); //cleanup the screen and go back to the menu state.

void playShortTone(int freq, int len); //play a tone on the buzzer.
void playRadioTone(int freq, int len); //play a tone on the radio.

void radioArmRadioTone(); //play radio message to indicate armed.
void gameOverRadioTone(); //play radio message to indicate game over.
void detonatedRadioTone(); //play radio message to indicate detonate.

void setup() {

  pinMode(buzzerPin, OUTPUT); //setup the buzzer pin. This is a tone pin
  pinMode(radioOutputPin, OUTPUT); //setup the radio output. This is a tone pin.
  pinMode(PTTPin, OUTPUT); //output on the PTTPin.
  digitalWrite(PTTPin, LOW); //write to PTT to disable

  #ifdef externalButton
  pinMode(switchPin,INPUT_PULLUP); //external button
  #endif

  lcd.begin(16, 2); // set up the LCD's number of columns and rows.
  
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
  game_time_set = gameTimes[GAME_time_default];

  //hold the position of the pointers to the array position
  arm_time_pos = ARM_time_default;
  trig_time_pos = TRIG_time_default;
  defuse_time_pos = DEFUSE_time_default;
  game_time_pos = GAME_time_default;

  //counter value variables
  current_time = 0;
  planting_time = 0;
  
  tone(buzzerPin,menuTone);
  printtoScreen("  TheRuddyBomb  ", VERSION);
  delay(1500);
  printtoScreen("  TheRuddyBomb  ", "  Device Ready  ");
  delay(500);
  noTone(buzzerPin);
  delay(500);
}

//main loop
void loop() {

  btn = getButton(); //get the button prompt.

  if (currentMode == MENU){
    menuSettings(); //do menu stuff
  }
  else if(currentMode == DEVICE_NEUTRAL){

    game_start_time = millis(); //set the game start time.
    
    //loop until one team captures the first position.
    while (true){

      btn = getButton(); //get the button.

      if ((millis() - game_start_time) >= 420000){
        //use tone to signal transition into sudden death.
        currentMode = COUNTING_DOWN;
        break;
      }

      //TODO
      //if button press: Check switch orientation andd go to that position.
      //play tone to indicate position capture.
      //if (button Press with red switch){

        currentMode == RED_TEAM_POS;
        break;

      //}
      //else if (button Press with green switch){

        currentMode = GREEN_TEAM_POS;
        break;

      //}
      
    }
    
  }
  else if(currentMode == RED_TEAM_POS){
  //red team possession. 

  unsigned long possession_start = millis();

  //add time to red team clock.

  //if time over 3 minutes then end.

    while (true){

    btn = getButton(); //get the button.
    
    //if button press and turn to green.
    if (buttonDown && switchinGreen){

      //subtract current score from red team.
      redPoints = redPoints - (millis() - possession_start);
      
      currentMode = GREEN_TEAM_POS;  
      break;
    }

    //if current time is greater than existing time - red team wins.
      if ((millis() - possession_start) >= redPoints){
        //red team wins.

        break;
      }
    }

  }
  else if(currentMode == GREEN_TEAM_POS){
  //green team possession.
  
  unsigned long possession_start = millis();

  //add time to green team clock.

  //if time over 3 minutes then end.

    while (true){

    //if button press and turn to red.
      if (switcheddown && switchinRED){
  
        greenPoints = greenPoints - (millis() - possession_start);
        
        //subtract current score from green team.
        currentMode = RED_TEAM_POS;  
        break;
      }

      //if current time is greater than existing time - green team wins.
      if ((millis() - possession_start) >= greenPoints){
        //green team wins.
        break;
      }
      
    }

  }
  else if (currentMode == COUNTING_DOWN){
  //No captures in first 7 minutes. Last position held wins in 3 minutes.

    unsigned long sd_timer_start = millis();

    //count down 3 minutes
    while (true){
  
      //if 3 minutes over, check switch orientation and award winner to that team.
      //sound tone.

      if ((millis() - sd_timer_start) >= 300000){
        //if (currently red) then red win goto redwinfn
        //if (currently green) then green win goto greenwinfn
        break;
      }

      //check the switch orientation to light up correct LED.
      //if (red) then red LED on
      //if (green) then geren LED on

      //if button press then switch allegence of device.
      //switch to whichever colour.

    }
    
  }
}

//On the start screen.
void preStartGame(){
  printtoTop("[PRESS TO START]");

  //set the game duration
  timerDisplay(game_time_set);

  //fill in the message
  global_timer_bar[2] = timerArray4[0];
  global_timer_bar[3] = timerArray4[1];
  global_timer_bar[5] = timerArray4[2];
  global_timer_bar[6] = timerArray4[3];

  //set the bomb duration
  timerDisplay(trig_time_set);

  global_timer_bar[10] = timerArray4[0];
  global_timer_bar[11] = timerArray4[1];
  global_timer_bar[13] = timerArray4[2];
  global_timer_bar[14] = timerArray4[3];

  printtoBot(global_timer_bar);
}

//start the game
//This function is the start process.
void startGame(){

  //TODO: WAIT 30 seconds before start.


  //Change: Removal of bomb beeping. All outputs are now through radio for start tone.
  printtoBot(global_timer_bar);

  //Countdown with sound feedback.
  digitalWrite(PTTPin,HIGH);
  delay(500); //let the open-mic signal propagate.
  
  printtoTop("[STARTING IN: 3]");
  //playShortTone(menuTone,500);
  //delay(100);
  tone(radioOutputPin, menuTone);
  delay(500);
  noTone(radioOutputPin);
  printtoTop("[STARTING IN: 2]");
  //playShortTone(menuTone,500);
  //delay(100);
  tone(radioOutputPin, menuTone);
  delay(500);
  noTone(radioOutputPin);
  printtoTop("[STARTING IN: 1]");
  //playShortTone(menuTone,500);
  //delay(100);
  tone(radioOutputPin, menuTone);
  delay(500);
  noTone(radioOutputPin);
  printtoTop("[  GAME START  ]");
  //playShortTone(startTone,500);
  //delay(100);
  tone(radioOutputPin, startTone);
  delay(500);
  noTone(radioOutputPin);

  delay(100); 
  digitalWrite(PTTPin,LOW); //end the mic signal out.

  //put an additional start tone to output.
  playShortTone(startTone,500);
  delay(100);

  //game start.
}

//set bomb parameters.
void menuSettings(){

  //print the current settings.
  preStartGame();
  
  //while true. Exit when the bomb is ready to be played.
  while (true){

    if (currentMenuMode == SET_RADIO_CHECK){
      noTone(buzzerPin); //safety check
      if ((millis() % 1500) == 0) playRadioTone(detonatorTone,500);
    }

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
        currentMenuMode = SET_GAME_TIME;
        printtoTop("Set Game Time   ");
        gameTimeAdjust(0);
      }
      else if (currentMenuMode == SET_GAME_TIME){
        currentMenuMode = SET_RADIO_CHECK;
        
        printtoTop("Radio Check     ");
        printtoBot("                ");
        
      }
      else if (currentMenuMode == SET_RADIO_CHECK){
        currentMenuMode = GAME_START;
        preStartGame();
      }
  
    }
    else if (btn == BUTTON_UP_PRESSED){
    
      playShortTone(menuTone,50);
      
      //loop the settings
      if (currentMenuMode == GAME_START){
        currentMenuMode = SET_RADIO_CHECK;
        
        printtoTop("Radio Check     ");
        printtoBot("                ");
        
      }
      else if (currentMenuMode == SET_ARM_TIME){
        currentMenuMode = GAME_START;
        preStartGame();
      }
      else if (currentMenuMode == SET_TRIG_TIME){
        currentMenuMode = SET_ARM_TIME;
        printtoTop("Set Arm Time    ");
        armTimeAdjust(0);
      }
      else if (currentMenuMode == SET_DEFUSE_TIME){
        currentMenuMode = SET_TRIG_TIME;
        printtoTop("Set Detonator   ");
        trigTimeAdjust(0);
      }
      else if (currentMenuMode == SET_GAME_TIME){
        currentMenuMode = SET_DEFUSE_TIME;
        printtoTop("Set Defuse Time ");
        defuseTimeAdjust(0);
      }
      else if (currentMenuMode == SET_RADIO_CHECK){
        currentMenuMode = SET_GAME_TIME;
        printtoTop("Set Game Time   ");
        gameTimeAdjust(0);
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
      else if (currentMenuMode == SET_GAME_TIME){
        gameTimeAdjust(-1);
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
      else if (currentMenuMode == SET_GAME_TIME){
        gameTimeAdjust(1);
      }
    }
    
    //check button press to start the game.
    #ifdef externalButton
    else if (digitalRead(switchPin) == 0){
    #else
    else if (btn == BUTTON_SELECT_PRESSED){
    #endif
    
      //Game starts here.
      if (currentMenuMode == GAME_START){
        
        //set the mode to bomb waiting to plant.
        currentMode = WAITING_FOR_PLANT;
        
        //start the game
        startGame();
        
        return;
      }
      
    } 
    
  }
  
}

//=====START PRINTING FUNCTIONS=====
void printtoScreen(char * top_row, char* bot_row){
  //print the stuff.
  lcd.setCursor(0, 0);
  lcd.print(top_row);
  lcd.setCursor(0, 1);
  lcd.print(bot_row);
}
void printtoTop(char* top_row){
  lcd.setCursor(0, 0);
  lcd.print(top_row);
}
void printtoBot(char* bot_row){
  lcd.setCursor(0, 1);
  lcd.print(bot_row);
}
void printtoScreen(const char * top_row,const char* bot_row){
  lcd.setCursor(0, 0);
  lcd.print(top_row);
  lcd.setCursor(0, 1);
  lcd.print(bot_row);
}
void printtoTop(const char* top_row){
  lcd.setCursor(0, 0);
  lcd.print(top_row);
}
void printtoBot(const char* bot_row){
  lcd.setCursor(0, 1);
  lcd.print(bot_row);
}
//=====END PRINTING FUNCTIONS=====

void armTimeAdjust(char mod){
  
  //already at the bounds of the array
  if ((arm_time_pos == 0 && mod == -1 )||(arm_time_pos == (ARM_time_count - 1) && mod == 1)){
    //do nothing to prevent increment past bounds.
    return;
  }

  if (mod == 1){
    ++arm_time_pos;
    arm_time_set = armTimes[arm_time_pos];
  }
  else if (mod == -1){
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

//adjust detonator time
void trigTimeAdjust(char mod){
    //already at the bounds of the array
  if ((trig_time_pos == 0 && mod == -1 )||(trig_time_pos == (TRIG_time_count - 1) && mod == 1)){
    //do nothing to prevent increment past bounds.
    return;
  }

  if (mod == 1){
    ++trig_time_pos;
    trig_time_set = trigTimes[trig_time_pos];
  }
  else if (mod == -1){
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

//adjust the defuse time
void defuseTimeAdjust(char mod){
  
  //already at the bounds of the array
  if ((defuse_time_pos == 0 && mod == -1 )||(defuse_time_pos == (DEFUSE_time_count - 1) && mod == 1)){
    //do nothing to prevent increment past bounds.
    return;
  }

  if (mod == 1){
    ++defuse_time_pos;
    defuse_time_set = defuseTimes[defuse_time_pos];
  }
  else if (mod == -1){
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

//adjust the global game time.
void gameTimeAdjust(char mod){
  
  //already at the bounds of the array
  if ((game_time_pos == 0 && mod == -1 )||(game_time_pos == (GAME_time_count - 1) && mod == 1)){
    //do nothing to prevent increment past bounds.
    return;
  }

  //change the setting
  if (mod == 1){
    ++game_time_pos;
    game_time_set = gameTimes[game_time_pos];
  }
  else if (mod == -1){
    --game_time_pos;
    game_time_set = gameTimes[game_time_pos];
  }

  //get the number in milliseconds to seconds, and print.
  Int2AsciiExt((unsigned int)(game_time_set/60000));

  //place the data into the array.
  temp_array_minute[3] = temp_array_3[1];
  temp_array_minute[4] = temp_array_3[0];

  //now print the values to the screen.
  //change the chevron direction.
  if (game_time_pos == 0){
    temp_array_minute[0] = ' ';
    temp_array_minute[15] = '>';
  }
  else if (game_time_pos == (GAME_time_count - 1)){
    temp_array_minute[0] = '<';
    temp_array_minute[15] = ' ';
  }
  else{
    temp_array_minute[0] = '<';
    temp_array_minute[15] = '>';
  }

  //print the setting output.
  printtoBot(temp_array_minute);
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
  while (tens > 9){ hundreds++; tens -= 10; }

  //Set the array.
  temp_array_3[0] = units + '0';
  temp_array_3[1] = tens + '0';
  temp_array_3[2] = hundreds + '0';
}

//convert milliseconds to displayable characters.
void timerDisplay(unsigned long input){

  char ten_minutes = 0, one_minute = 0, ten_seconds = 0, one_second = 0;

  //10 min
  while (input >= 600000){ ++ten_minutes; input -= 600000; }
  //1 min
  while (input >= 60000){ ++one_minute; input -= 60000; }
  //10 secs
  while (input >= 10000){ ++ten_seconds; input -= 10000; }
  //1 sec
  while (input >= 1000){ ++one_second; input -= 1000; }

  //output to the array
  timerArray4[0] = ten_minutes + '0';
  timerArray4[1] = one_minute + '0';
  timerArray4[2] = ten_seconds + '0';
  timerArray4[3] = one_second + '0';
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
void defendersDefuse(unsigned long BombTime){
  
  //calculate the remaining time.
  unsigned long bomb_remaining_time = trig_time_set - (millis() - BombTime);
  unsigned long clock_remaining_time =  game_time_set - (millis() - game_start_time);

  //now add the values to the global timer bar.
  timerDisplay(clock_remaining_time);

  //fill in the message
  global_timer_bar[2] = timerArray4[0];
  global_timer_bar[3] = timerArray4[1];
  global_timer_bar[5] = timerArray4[2];
  global_timer_bar[6] = timerArray4[3];

  //set the bomb duration
  timerDisplay(bomb_remaining_time);

  global_timer_bar[10] = timerArray4[0];
  global_timer_bar[11] = timerArray4[1];
  global_timer_bar[13] = timerArray4[2];
  global_timer_bar[14] = timerArray4[3];

  //print notification
  printtoTop("[   DEFUSED!   ]");
  printtoBot(global_timer_bar);
  
  digitalWrite(PTTPin,LOW);
  noTone(radioOutputPin);
  noTone(buzzerPin);
  gameOverRadioTone(); //output on radio.

  //descending tones.
  for (int i = 400; i >= 100; --i){
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

  digitalWrite(PTTPin,LOW);
  noTone(radioOutputPin);
  noTone(buzzerPin);
  detonatedRadioTone(); //radio output
  
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
  
  digitalWrite(PTTPin,LOW);
  noTone(radioOutputPin);
  noTone(buzzerPin);
  gameOverRadioTone(); //end the game over radio.
  
  //play game end tone.
  tone(buzzerPin, menuTone);
  

  //cleanup stuff
  endOfGameCleanup();
}

void foulDetonate(){

  digitalWrite(PTTPin,LOW);
  noTone(radioOutputPin);
  noTone(buzzerPin);
  detonatedRadioTone(); //end the game.
    
  //play detonation tone.
  tone(buzzerPin, detonatorTone);
  
  //print notification.
  printtoTop("[     FOUL!    ]");
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

  #ifdef externalButton
  while (digitalRead(switchPin) == 0);
  while (digitalRead(switchPin) == 1);
  #else
  while (!(btn == BUTTON_RIGHT_PRESSED)) btn = getButton();
  #endif

  delay(1000); //wait 1 second to prevent fallthrough.
  
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

//=====START TONE SEQUENCES ====
void playShortTone(int freq, int len){
  tone(buzzerPin,freq); //turn on buzzer
  delay(len); //do a delay
  noTone(buzzerPin); //turn off buzzer
}
void playRadioTone(int freq, int len){
  digitalWrite(PTTPin,HIGH);
  delay(100);
  tone(radioOutputPin, freq);
  delay(len);
  noTone(radioOutputPin);
  delay(50);
  digitalWrite(PTTPin,LOW);
}
void radioArmRadioTone(){
  digitalWrite(PTTPin,HIGH);
  delay(100);
  for (int i = 100; i < 400; ++i){
    tone(radioOutputPin,i);
    delay(5);
  }
  noTone(radioOutputPin);
  digitalWrite(PTTPin,LOW);
}
void gameOverRadioTone(){
  digitalWrite(PTTPin,HIGH);
  delay(100);
  for (int i = 400; i > 100; --i){
    tone(radioOutputPin,i);
    delay(5);
  }
  noTone(radioOutputPin);
  digitalWrite(PTTPin,LOW);
}
void detonatedRadioTone(){
  digitalWrite(PTTPin,HIGH);
  delay(100);
  for (int i = 0; i < 3; ++i){
    tone(radioOutputPin,262);
    delay(800);
    tone(radioOutputPin,400);
    delay(900);
    noTone(radioOutputPin);
    delay(500);
  }
  noTone(radioOutputPin);
  digitalWrite(PTTPin,LOW);
}
//=====END RADIO TONE SEQUENCES=====
