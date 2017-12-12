#include <LiquidCrystal.h>
#include <display/Config.h>
#include <display/LcdKeypad.h>
#include <display/menu/MenuData.h>
#include "TheRuddyBomb.h"

byte p1[8] = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10};

byte p2[8] = {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18};

byte p3[8] = {0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C};

byte p4[8] = {0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E};

byte p5[8] = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};

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

long timerCurrentValue[3];
short timerFineGrainedCounter[3];
unsigned long lastMilliSecondTimerValue = 0;
int currentTimerIdx = 0;
byte btn;
Config currentConfig;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

MenuManager Menu1(cd_timer_menu_Root, menuCount(cd_timer_menu_Root));

unsigned long bombPlantStart;
unsigned long bombDefuseStart;

uint8_t buzzerPin = 3; // the buzzer pin
boolean tock = false;

#define length 16.0

double percent = 100.0;
unsigned char existingDrawn;
unsigned int character;

bool availableForDefuse = false;

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
    lcd.setCursor(0, 1);
    lcd.print("Bomb ready");

    initTimers();

    Serial.begin(9600);
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

            if (btn == BUTTON_SELECT_PRESSED || btn == BUTTON_SELECT_LONG_PRESSED) {
                initiateBombPlant();
            }
            break;
        case BOMB_PLANTING :
            if (btn == BUTTON_SELECT_LONG_RELEASE) {
                cancelBombPlant();
            } else if (btn == BUTTON_SELECT_PRESSED || btn == BUTTON_SELECT_LONG_PRESSED) {
                plantBomb();
            }
            break;
        case BOMB_DEFUSING :
//            if (btn == BUTTON_SELECT_SHORT_RELEASE || btn == BUTTON_SELECT_LONG_RELEASE) {
            Serial.println("Bomb defusing");
//                bombDefuseStart = 0;
//
//                lcd.clear();
//                appMode = TIMER_RUNNING;
//            } else {
//                printDefuseTimeRemainder();
//
//                if (currentConfig.bombDefuseTime <= ((millis() - bombDefuseStart) / 1000)) {
//                    lcd.clear();
//                    lcd.setCursor(0, 0);
//                    lcd.print("Bomb defused!");
//
//                    lastMilliSecondTimerValue = 0;
//                    appMode = WAITING_FOR_PLANT;
//                }
//            }

            break;
        case TIMER_RUNNING : {

            if (btn == BUTTON_SELECT_PRESSED && availableForDefuse) {
                startDefuse();
                break;
            } else {
                auto msDelta = (short) (millis() - lastMilliSecondTimerValue);

                if (msDelta > 0) {
                    lastMilliSecondTimerValue = millis();
                    timerFineGrainedCounter[currentTimerIdx] += msDelta;

                    Serial.print("Last mili timer (ms): ");
                    Serial.println(lastMilliSecondTimerValue);

                    Serial.print("Timer counter (ms): ");
                    Serial.println(timerFineGrainedCounter[currentTimerIdx]);

                    if (timerFineGrainedCounter[currentTimerIdx] >= 1000) {

                        tickDownTimer();

                        if (timerCurrentValue[currentTimerIdx] <= 0) {
                            finishTimer();
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
                    char strbuf[LCD_COLS + 1]; // one line of lcd display
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
                char strbuf[LCD_COLS + 1]; // one line of lcd display
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

void initiateBombPlant() {
    Serial.println("Initiataing bomb plant");
    lcd.clear();

    bombPlantStart = millis();

    printPlantTimeRemainder();

    appMode = BOMB_PLANTING;
}

void cancelBombPlant() {
    Serial.println("Bomb planting stopped");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Plant the bomb!");

    appMode = WAITING_FOR_PLANT;
}

void plantBomb() {
    Serial.println("Bomb planting");

    printPlantTimeRemainder();

    if (currentConfig.bombArmTime <= ((millis() - bombPlantStart) / 1000)) {
        Serial.println("Bomb planted");
        bombPlantStart = 0;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Bomb planted!");

        lastMilliSecondTimerValue = millis();
        appMode = TIMER_RUNNING;
    }
}

void startDefuse() {
    Serial.print("Starting defuse");
    lcd.clear();

    bombDefuseStart = millis();

    printDefuseTimeRemainder();

    appMode = BOMB_DEFUSING;
}

void tickDownTimer() {
    availableForDefuse = true; // wait 1 second and then say that people can defuse

    playBombTickSound();

    timerFineGrainedCounter[currentTimerIdx] -= 1000;
    timerCurrentValue[currentTimerIdx] -= 1;

    Serial.print("Current timer value remaining (s): ");
    Serial.println(timerCurrentValue[currentTimerIdx]);

    printTimerValue(currentTimerIdx);
}

void finishTimer() {
    Serial.println("Timer finished");
    timerCurrentValue[currentTimerIdx] = currentConfig.getTimerReloadValue(currentTimerIdx);


    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Game over man");
    lcd.setCursor(0, 1);
    lcd.print("terrorists win");

    tone(buzzerPin, 1000, 1000); // your'e dead

    delay(2000);

    appMode = SELECT_BOMB_TYPE;
}

void printPlantTimeRemainder() {
    Serial.println("Printing plant time remainder");

    lcd.setCursor(0, 0);

    char displaySecondsBuf[2];
    auto plantRemainingTime = static_cast<int>((millis() - bombPlantStart) / 1000);

    Serial.print("Plant remainder (s)");
    Serial.println(plantRemainingTime);

    char plantStrbuf[LCD_COLS + 1]; // one line of lcd display
    char *plantTime = fmt(plantStrbuf, 2, "Planting.. ",
                          inttostr(displaySecondsBuf, currentConfig.bombArmTime - plantRemainingTime));

    lcd.print(rpad(plantStrbuf, plantTime));
    drawProgress(plantRemainingTime, (double) currentConfig.bombArmTime);
}

// TODO: Something in here is still breaking everything
void printDefuseTimeRemainder() {
    Serial.println("Printing defuse time remainder");

    lcd.setCursor(0, 0);

    char displayDefuseSecondsBuf[2];
    auto defuseRemainingTime = static_cast<int>((millis() - bombDefuseStart) / 1000);

    Serial.print("Defuse remainder (s)");
    Serial.println(defuseRemainingTime);

    char defuseStrbuf[LCD_COLS + 1]; // one line of lcd display
    char *defuseTime = fmt(defuseStrbuf, 2, "Defusing.. ",
                           inttostr(displayDefuseSecondsBuf, currentConfig.bombDefuseTime - defuseRemainingTime));

    lcd.print(rpad(defuseStrbuf, defuseTime));
    drawProgress(defuseRemainingTime, (double) currentConfig.bombDefuseTime);
}

void drawProgress(unsigned long currTime, double maxTime) {

    lcd.setCursor(0, 1);

    percent = currTime / maxTime * 100.0;

    Serial.print("Progress (%): ");
    Serial.println(percent);

    double colsToFill = length / 100 * percent;

    if (colsToFill >= 1) {
        for (int i = 1; i <= colsToFill; i++) {
            lcd.print((char) 4);
            existingDrawn = i;
        }
        colsToFill = colsToFill - existingDrawn;
    }
    character = colsToFill * 5;

    switch (character) {

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
            Serial.println("ERROR - Unknown character to print");
            break;
    }

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
void printTimerValue(int timerIdx, bool showTimerName) {
    char strbuf[LCD_COLS + 1]; // one line of lcd display
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

    bombDefuseStart = 0;
    bombPlantStart = 0;
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
        char strbuf[LCD_COLS + 1]; // one line of lcd display
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
    char strbuf[LCD_COLS + 1]; // one line of lcd display

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

