#include <LiquidCrystal.h>
#include <display/Config.h>
#include <display/LcdKeypad.h>
#include <display/menu/MenuData.h>
#include "TheRuddyBomb.h"

byte p1[8] = {
        0x10,
        0x10,
        0x10,
        0x10,
        0x10,
        0x10,
        0x10,
        0x10};

byte p2[8] = {
        0x18,
        0x18,
        0x18,
        0x18,
        0x18,
        0x18,
        0x18,
        0x18};

byte p3[8] = {
        0x1C,
        0x1C,
        0x1C,
        0x1C,
        0x1C,
        0x1C,
        0x1C,
        0x1C};

byte p4[8] = {
        0x1E,
        0x1E,
        0x1E,
        0x1E,
        0x1E,
        0x1E,
        0x1E,
        0x1E};

byte p5[8] = {
        0x1F,
        0x1F,
        0x1F,
        0x1F,
        0x1F,
        0x1F,
        0x1F,
        0x1F};

enum AppModeValues {
    SELECT_BOMB_TYPE,
    WAITING_FOR_PLANT,
    BOMB_PLANTING,
    BOMB_DEFUSING,
    TIMER_RUNNING,
    APP_MENU_MODE,
    APP_PROCESS_MENU_CMD,
    APP_MENU_MODE_END
};

byte appMode = SELECT_BOMB_TYPE;

char strbuf[LCD_COLS + 1]; // one line of lcd display
long timerCurrentValue[3];
short timerFineGrainedCounter[3];
unsigned long lastMilliSecondTimerValue = 0;
char currentTimerIdx = 0;
byte btn;
Config currentConfig;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

MenuManager Menu1(cd_timer_menu_Root, menuCount(cd_timer_menu_Root));

unsigned long bombPlantStart;
unsigned long bombDefuseStart;

void printTimerValue(byte timerIdx, bool showTimerName = false);

void printPlantTimeRemainder();

void printDefuseTimeRemainder();

void drawProgress(unsigned long currTime, double maxTime);

uint8_t buzzerPin = 3; // the buzzer pin
boolean tock = false;

#define length 16.0

double percent = 100.0;
unsigned char b;
unsigned int piece;

void playBombTickSound() {

    if (currentConfig.countDownBeepConstant || timerCurrentValue[currentTimerIdx] <= 10) {
        if (!tock) {
            tone(buzzerPin, 900, 500);

            tock = true;
        } else {
            tone(buzzerPin, 600, 500);

            tock = false;
        }
    }
}


void setup() {

    pinMode(buzzerPin, OUTPUT);

    backLightOn();
    // set up the LCD's number of columns and rows:

    lcd.createChar(0, p1);
    lcd.createChar(1, p2);
    lcd.createChar(2, p3);
    lcd.createChar(3, p4);
    lcd.createChar(4, p5);

    lcd.begin(LCD_COLS, LCD_ROWS);
    currentConfig.load();
    setBacklightBrightness(currentConfig.displayBrightness);

    lcd.setCursor(0, 0);
    lcd.print("TheRuddyBomb");
    lcd.setCursor(0, 1);
    lcd.print("v 7.7.7");
    delay(1500);

    initTimers();
}

void drawProgress(unsigned long currTime, double maxTime) {

    lcd.setCursor(0, 1);

    percent = currTime / maxTime * 100.0;

    double a = length / 100 * percent;

    if (a >= 1) {
        for (int i = 1; i < a; i++) {
            lcd.print((char) 4);
            b = i;
        }
        a = a - b;
    }
    piece = a * 5;

    switch (piece) {

        case 0:
            break;

        case 1:
            lcd.print((char) 0);
            break;

        case 2:
            lcd.print((char) 1);
            break;

        case 3:
            lcd.print((char) 2);
            break;

        case 4:
            lcd.print((char) 3);
            break;

        default:
//            lcd.print("ERROR");
            break;
    }

}

void loop() {
    btn = getButton();

    if (btn && currentConfig.buttonBeep) {
        byte btnFlags = btn & 192;

        if (btnFlags == BUTTON_PRESSED_IND)   // if any button pressed.
        {
            tone(buzzerPin, 200, 100);
        }
    }

    switch (appMode) {
        case SELECT_BOMB_TYPE :
            // start selected timer
            if (btn == BUTTON_SELECT_SHORT_RELEASE) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Plant the bomb!");
                appMode = WAITING_FOR_PLANT;
            } else if (btn == BUTTON_UP_SHORT_RELEASE) {
                currentTimerIdx = static_cast<char>(--currentTimerIdx < 0 ? 0 : currentTimerIdx);
                printTimerValue(currentTimerIdx, true);
            } else if (btn == BUTTON_DOWN_SHORT_RELEASE) {
                currentTimerIdx = ++currentTimerIdx > 2 ? 2 : currentTimerIdx;
                printTimerValue(currentTimerIdx, true);
            } else if (btn == BUTTON_UP_LONG_PRESSED) {
                refreshMenuDisplay(REFRESH_DESCEND);
                appMode = APP_MENU_MODE;
            }
            break;
        case WAITING_FOR_PLANT :
            // game started. wait for someone to plant the bomb
            initTimers();

            if (btn == BUTTON_SELECT_SHORT_RELEASE) {
                bombPlantStart = millis();
                lcd.clear();

                printPlantTimeRemainder();

                lcd.setCursor(0, 1);
                appMode = BOMB_PLANTING;
            }
            break;
        case BOMB_PLANTING :
            if (btn == BUTTON_SELECT_SHORT_RELEASE) {
                bombDefuseStart = millis();

                lcd.clear();
                printDefuseTimeRemainder();

                appMode = BOMB_DEFUSING;
                break;
            }

            printPlantTimeRemainder();
            if (currentConfig.bombArmTime <= ((millis() - bombPlantStart) / 1000)) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Bomb planted!");

                lastMilliSecondTimerValue = millis();
                appMode = TIMER_RUNNING;
            }
            break;
        case BOMB_DEFUSING :
            if (btn == BUTTON_SELECT_SHORT_RELEASE) {
                bombPlantStart = millis();

                lcd.clear();
                printPlantTimeRemainder();

                appMode = BOMB_PLANTING;
                break;
            }

            printDefuseTimeRemainder();
            if (currentConfig.bombDefuseTime <= ((millis() - bombDefuseStart) / 1000)) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Bomb defused!");

                lastMilliSecondTimerValue = 0;
                appMode = WAITING_FOR_PLANT;
            }
            break;
        case TIMER_RUNNING : {
            if (btn == BUTTON_SELECT_LONG_RELEASE) {
                bombDefuseStart = millis();

                lcd.clear();
                printDefuseTimeRemainder();

                appMode = BOMB_DEFUSING;
            } else {
                short msDelta = (millis() - lastMilliSecondTimerValue);

                if (msDelta > 0) {
                    lastMilliSecondTimerValue = millis();
                    timerFineGrainedCounter[currentTimerIdx] += msDelta;

                    if (timerFineGrainedCounter[currentTimerIdx] >= 1000) {

                        playBombTickSound();

                        timerFineGrainedCounter[currentTimerIdx] -= 1000;
                        timerCurrentValue[currentTimerIdx] -= 1;
                        printTimerValue(currentTimerIdx);

                        if (timerCurrentValue[currentTimerIdx] <= 0) {
                            timerCurrentValue[currentTimerIdx] = currentConfig.getTimerReloadValue(currentTimerIdx);

                            appMode = SELECT_BOMB_TYPE;

                            lcd.clear();
                            lcd.setCursor(0, 0);
                            lcd.print("Game over man");
                            lcd.setCursor(0, 1);
                            lcd.print("terrorists win");

                            tone(buzzerPin, 1000, 1000); // your'e dead

                            delay(2000);

                            break;
                        }
                    }
                }
            }
            break;
        }
        case APP_MENU_MODE : {
            byte menuMode = Menu1.handleNavigation(getNavAction, refreshMenuDisplay);

            if (menuMode == MENU_EXIT) {
                // Tidy up display
                printTimerValue(currentTimerIdx, true);
                appMode = APP_MENU_MODE_END;
            } else if (menuMode == MENU_INVOKE_ITEM) {
                appMode = APP_PROCESS_MENU_CMD;

                // Highlight selected item (if not reset command).
                if (Menu1.getCurrentItemCmdId() != mnuCmdResetToDefaults) {
                    lcd.setCursor(0, 1);
                    strbuf[0] = 0b01111110; // forward arrow representing input prompt.
                    strbuf[1] = 0;
                    lcd.print(strbuf);
                }
            }
            break;
        }

        case APP_PROCESS_MENU_CMD : {
            bool processingComplete = processMenuCommand(Menu1.getCurrentItemCmdId());

            if (processingComplete) {
                appMode = APP_MENU_MODE;
                // Unhighlight selected item
                lcd.setCursor(0, 1);
                strbuf[0] = ' '; // clear forward arrow
                strbuf[1] = 0;
                lcd.print(strbuf);
            }
            break;
        }
        case APP_MENU_MODE_END : {
            if (btn == BUTTON_SELECT_SHORT_RELEASE || btn == BUTTON_SELECT_LONG_RELEASE) {
                appMode = SELECT_BOMB_TYPE;
            }
            break;
        }
    }

}


void printPlantTimeRemainder() {
    lcd.setCursor(0, 0);
    char displaySecondsBuf[2];
    unsigned long remainingTime = (millis() - bombPlantStart) / 1000;
    char *plantTime = fmt(strbuf, 2, "Planting.. ",
                          inttostr(displaySecondsBuf, currentConfig.bombArmTime - remainingTime));

    lcd.print(rpad(strbuf, plantTime));
    drawProgress(remainingTime, (double) currentConfig.bombArmTime);
}

void printDefuseTimeRemainder() {
    lcd.setCursor(0, 0);
    char displaySecondsBuf[2];
    unsigned long remainingTime = (millis() - bombDefuseStart) / 1000;
    char *defuseTime = fmt(strbuf, 2, "Defusing.. ",
                           inttostr(displaySecondsBuf, currentConfig.bombDefuseTime - remainingTime));

    lcd.print(rpad(strbuf, defuseTime));
    drawProgress(remainingTime, (double) currentConfig.bombDefuseTime);
}

//----------------------------------------------------------------------
// Callback to convert button press to navigation action.
byte getNavAction() {
    byte navAction = 0;
    byte currentItemHasChildren = Menu1.currentItemHasChildren();

    if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED) navAction = MENU_ITEM_PREV;
    else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED) navAction = MENU_ITEM_NEXT;
    else if (btn == BUTTON_SELECT_PRESSED || (btn == BUTTON_RIGHT_PRESSED && currentItemHasChildren))
        navAction = MENU_ITEM_SELECT;
    //else if (btn == BUTTON_LEFT_PRESSED) navAction = MENU_BACK;
    return navAction;
}


//----------------------------------------------------------------------
void printTimerValue(byte timerIdx, bool showTimerName) {
    if (showTimerName) {
        lcd.clear();
        lcd.setCursor(0, 0);
        char intbuf[2];

        inttostr(intbuf, timerIdx + 1);

        fmt(strbuf, 2, "Bomb ", intbuf);
        lcd.print(strbuf);
    }

    lcd.setCursor(0, 1);
    toTimeStr(strbuf, timerCurrentValue[timerIdx]);
    lcd.print(strbuf);
}


void initTimers() {
    lastMilliSecondTimerValue = 0;

    timerCurrentValue[0] = currentConfig.timer1ReloadValue;
    timerCurrentValue[1] = currentConfig.timer2ReloadValue;
    timerCurrentValue[2] = currentConfig.timer3ReloadValue;

    timerFineGrainedCounter[0] = 0;
    timerFineGrainedCounter[1] = 0;
    timerFineGrainedCounter[2] = 0;
}


//----------------------------------------------------------------------
// Addition or removal of menu items in MenuData.h will require this method
// to be modified accordingly.
bool processMenuCommand(byte cmdId) {
    bool complete = false;  // set to true when menu command processing complete.
    bool configChanged = false;

    if (btn == BUTTON_SELECT_PRESSED) {
        if (cmdId != mnuCmdResetToDefaults) complete = true;
    }

    switch (cmdId) {
        // Process menu commands here:
        case mnuCmdT1Hours :
            configChanged = true;
            if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED) {
                currentConfig.timer1ReloadValue = addToTime(3600, currentConfig.timer1ReloadValue);
            } else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED) {
                currentConfig.timer1ReloadValue = addToTime(-3600, currentConfig.timer1ReloadValue);
            } else {
                configChanged = false;
            }
            break;
        case mnuCmdT1Mins :
            configChanged = true;
            if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED) {
                currentConfig.timer1ReloadValue = addToTime(60, currentConfig.timer1ReloadValue);
            } else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED) {
                currentConfig.timer1ReloadValue = addToTime(-60, currentConfig.timer1ReloadValue);
            } else {
                configChanged = false;
            }
            break;
        case mnuCmdT1Secs :
            configChanged = true;
            if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED) {
                currentConfig.timer1ReloadValue = addToTime(1, currentConfig.timer1ReloadValue);
            } else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED) {
                currentConfig.timer1ReloadValue = addToTime(-1, currentConfig.timer1ReloadValue);
            } else {
                configChanged = false;
            }
            break;
        case mnuCmdT2Hours :
            configChanged = true;
            if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED) {
                currentConfig.timer2ReloadValue = addToTime(3600, currentConfig.timer2ReloadValue);
            } else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED) {
                currentConfig.timer2ReloadValue = addToTime(-3600, currentConfig.timer2ReloadValue);
            } else {
                configChanged = false;
            }
            break;
        case mnuCmdT2Mins :
            configChanged = true;
            if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED) {
                currentConfig.timer2ReloadValue = addToTime(60, currentConfig.timer2ReloadValue);
            } else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED) {
                currentConfig.timer2ReloadValue = addToTime(-60, currentConfig.timer2ReloadValue);
            } else {
                configChanged = false;
            }
            break;
        case mnuCmdT2Secs :
            configChanged = true;
            if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED) {
                currentConfig.timer2ReloadValue = addToTime(1, currentConfig.timer2ReloadValue);
            } else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED) {
                currentConfig.timer2ReloadValue = addToTime(-1, currentConfig.timer2ReloadValue);
            } else {
                configChanged = false;
            }
            break;
        case mnuCmdT3Hours :
            configChanged = true;
            if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED) {
                currentConfig.timer3ReloadValue = addToTime(3600, currentConfig.timer3ReloadValue);
            } else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED) {
                currentConfig.timer3ReloadValue = addToTime(-3600, currentConfig.timer3ReloadValue);
            } else {
                configChanged = false;
            }
            break;
        case mnuCmdT3Mins :
            configChanged = true;
            if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED) {
                currentConfig.timer3ReloadValue = addToTime(60, currentConfig.timer3ReloadValue);
            } else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED) {
                currentConfig.timer3ReloadValue = addToTime(-60, currentConfig.timer3ReloadValue);
            } else {
                configChanged = false;
            }
            break;
        case mnuCmdT3Secs :
            configChanged = true;
            if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED) {
                currentConfig.timer3ReloadValue = addToTime(1, currentConfig.timer3ReloadValue);
            } else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED) {
                currentConfig.timer3ReloadValue = addToTime(-1, currentConfig.timer3ReloadValue);
            } else {
                configChanged = false;
            }
            break;
        case mnuCmdButtonBeep:
            configChanged = true;
            if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED) {
                currentConfig.buttonBeep = true;
            } else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED) {
                currentConfig.buttonBeep = false;
            } else {
                configChanged = false;
            }
            break;
        case mnuCmdCountdownBeepConstant:
            configChanged = true;
            if (btn == BUTTON_UP_PRESSED) {
                currentConfig.countDownBeepConstant = true;
            } else if (btn == BUTTON_DOWN_PRESSED) {
                currentConfig.countDownBeepConstant = false;
            } else {
                configChanged = false;
            }
            break;
        case mnuCmdDisplayBrightness :
            configChanged = true;
            if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED) {
                currentConfig.displayBrightness++;
                currentConfig.displayBrightness = constrain(currentConfig.displayBrightness, 1, 4);
                setBacklightBrightness(currentConfig.displayBrightness);
            } else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED) {
                currentConfig.displayBrightness--;
                currentConfig.displayBrightness = constrain(currentConfig.displayBrightness, 1, 4);
                setBacklightBrightness(currentConfig.displayBrightness);
            } else {
                configChanged = false;
            }
            break;
        case mnuCmdBombArmTime:
            configChanged = true;
            if (btn == BUTTON_UP_PRESSED) {
                currentConfig.bombArmTime = ++currentConfig.bombArmTime;
            } else if (btn == BUTTON_UP_LONG_PRESSED) {
                currentConfig.bombArmTime += 10;
            } else if (btn == BUTTON_DOWN_PRESSED) {
                currentConfig.bombArmTime = --currentConfig.bombArmTime;
            } else if (btn == BUTTON_DOWN_LONG_PRESSED) {
                currentConfig.bombArmTime -= 10;
            } else {
                configChanged = false;
            }
            break;
        case mnuCmdBombDefuseTime:
            configChanged = true;
            if (btn == BUTTON_UP_PRESSED) {
                currentConfig.bombDefuseTime = ++currentConfig.bombDefuseTime;
            } else if (btn == BUTTON_UP_LONG_PRESSED) {
                currentConfig.bombDefuseTime += 10;
            } else if (btn == BUTTON_DOWN_PRESSED) {
                currentConfig.bombDefuseTime = --currentConfig.bombDefuseTime;
            } else if (btn == BUTTON_DOWN_LONG_PRESSED) {
                currentConfig.bombDefuseTime -= 10;
            } else {
                configChanged = false;
            }
            break;
        case mnuCmdResetToDefaults:
            if (btn == BUTTON_SELECT_LONG_PRESSED) {
                currentConfig.setDefaults();
                setBacklightBrightness(currentConfig.displayBrightness);
                lcd.setCursor(1, 1);
                lcd.print("Defaults loaded");
            } else if (/*btn == BUTTON_SELECT_SHORT_RELEASE ||*/ btn == BUTTON_SELECT_LONG_RELEASE) {
                complete = true;
            }
            break;
        default:
            break;
    }

    if (configChanged && cmdId != mnuCmdResetToDefaults) {
        lcd.setCursor(1, 1);
        lcd.print(rpad(strbuf, currentConfig.getFormattedStr(cmdId))); // Display config value.
    }
    if (complete) {
        currentConfig.save();
        initTimers();
    }
    return complete;
}

//----------------------------------------------------------------------
const char EmptyStr[] = "";

// Callback to refresh display during menu navigation, using parameter of type enum DisplayRefreshMode.
void refreshMenuDisplay(byte refreshMode) {
    char nameBuf[LCD_COLS + 1];

    lcd.setCursor(0, 0);
    if (Menu1.currentItemHasChildren()) {
        rpad(strbuf, Menu1.getCurrentItemName(nameBuf));
        strbuf[LCD_COLS - 1] = 0b01111110;            // Display forward arrow if this menu item has children.
        lcd.print(strbuf);
        lcd.setCursor(0, 1);
        lcd.print(rpad(strbuf, EmptyStr));          // Clear config value in display
    } else {
        byte cmdId;
        rpad(strbuf, Menu1.getCurrentItemName(nameBuf));

        if ((cmdId = Menu1.getCurrentItemCmdId()) == 0) {
            strbuf[LCD_COLS - 1] = 0b01111111;          // Display back arrow if this menu item ascends to parent.
            lcd.print(strbuf);
            lcd.setCursor(0, 1);
            lcd.print(rpad(strbuf, EmptyStr));        // Clear config value in display.
        } else {
            lcd.print(strbuf);
            lcd.setCursor(0, 1);
            lcd.print(" ");
            lcd.print(rpad(strbuf, currentConfig.getFormattedStr(cmdId))); // Display config value.
        }
    }
}

