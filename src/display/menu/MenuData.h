#ifndef _cd_timer_menu_
#define _cd_timer_menu_

#include <avr/pgmspace.h>
#include <MenuManager.h>

/*
Generated using LCD Menu Builder at http://lcd-menu-bulder.cohesivecomputing.co.uk/.
*/

enum cd_timer_menuCommandId {
    mnuCmdBack = 0,
    mnuCmdBomb1Root,
    mnuCmdT1Hours,
    mnuCmdT1Mins,
    mnuCmdT1Secs,
    mnuCmdBomb2Root,
    mnuCmdT2Hours,
    mnuCmdT2Mins,
    mnuCmdT2Secs,
    mnuCmdBomb3Root,
    mnuCmdT3Hours,
    mnuCmdT3Mins,
    mnuCmdT3Secs,
    mnuCmdAlarmDuration,
    mnuCmdButtonBeep,
    mnuCmdCountdownBeepConstant,
    mnuCmdDisplayBrightness,
    mnuCmdBombArmTime,
    mnuCmdBombDefuseTime,
    mnuCmdResetToDefaults
};

PROGMEM const char cd_timer_menu_back[] = "Back";
PROGMEM const char cd_timer_menu_exit[] = "Exit";

PROGMEM const char cd_timer_menu_1_1[] = "Bomb1 set hrs";
PROGMEM const char cd_timer_menu_1_2[] = "Bomb1 set mins";
PROGMEM const char cd_timer_menu_1_3[] = "Bomb1 set secs";
PROGMEM const MenuItem cd_timer_menu_List_1[] = {
        {mnuCmdT1Hours, cd_timer_menu_1_1},
        {mnuCmdT1Mins,  cd_timer_menu_1_2},
        {mnuCmdT1Secs,  cd_timer_menu_1_3},
        {mnuCmdBack,    cd_timer_menu_back}
};

PROGMEM const char cd_timer_menu_2_1[] = "Bomb2 set hrs";
PROGMEM const char cd_timer_menu_2_2[] = "Bomb2 set mins";
PROGMEM const char cd_timer_menu_2_3[] = "Bomb2 set secs";
PROGMEM const MenuItem cd_timer_menu_List_2[] = {
        {mnuCmdT2Hours, cd_timer_menu_2_1},
        {mnuCmdT2Mins,  cd_timer_menu_2_2},
        {mnuCmdT2Secs,  cd_timer_menu_2_3},
        {mnuCmdBack,    cd_timer_menu_back}
};

PROGMEM const char cd_timer_menu_3_1[] = "Bomb3 set hrs";
PROGMEM const char cd_timer_menu_3_2[] = "Bomb3 set mins";
PROGMEM const char cd_timer_menu_3_3[] = "Bomb3 set secs";
PROGMEM const MenuItem cd_timer_menu_List_3[] = {
        {mnuCmdT3Hours, cd_timer_menu_3_1},
        {mnuCmdT3Mins,  cd_timer_menu_3_2},
        {mnuCmdT3Secs,  cd_timer_menu_3_3},
        {mnuCmdBack,    cd_timer_menu_back}
};

PROGMEM const char cd_timer_menu_1[] = "Bomb1 Profile";
PROGMEM const char cd_timer_menu_2[] = "Bomb2 Profile";
PROGMEM const char cd_timer_menu_3[] = "Bomb3 Profile";
PROGMEM const char cd_timer_menu_4[] = "Alarm Duration";
PROGMEM const char cd_timer_menu_5[] = "Button Beep";
PROGMEM const char cd_timer_menu_6[] = "Countdown beep";
PROGMEM const char cd_timer_menu_7[] = "LCD backlight";
PROGMEM const char cd_timer_menu_8[] = "Bomb arm time";
PROGMEM const char cd_timer_menu_9[] = "Bomb defuse time";
PROGMEM const char cd_timer_menu_reset[] = "Reset";

PROGMEM const MenuItem cd_timer_menu_Root[] = {{mnuCmdBomb1Root,                cd_timer_menu_1, cd_timer_menu_List_1,
                                                       menuCount(cd_timer_menu_List_1)
                                               },
                                               {mnuCmdBomb2Root,                cd_timer_menu_2, cd_timer_menu_List_2,
                                                       menuCount(cd_timer_menu_List_2)
                                               },
                                               {mnuCmdBomb3Root,                cd_timer_menu_3, cd_timer_menu_List_3,
                                                       menuCount(cd_timer_menu_List_3)
                                               },
                                               {mnuCmdAlarmDuration,            cd_timer_menu_4},
                                               {mnuCmdButtonBeep,               cd_timer_menu_5},
                                               {mnuCmdCountdownBeepConstant,    cd_timer_menu_6},
                                               {mnuCmdDisplayBrightness,        cd_timer_menu_7},
                                               {mnuCmdBombArmTime,              cd_timer_menu_8},
                                               {mnuCmdBombDefuseTime,           cd_timer_menu_9},
                                               {mnuCmdResetToDefaults,          cd_timer_menu_reset},
                                               {mnuCmdBack,                     cd_timer_menu_exit}};

#endif

