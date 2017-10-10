#ifndef CONFIG_H_
#define CONFIG_H_

#include "Arduino.h"

const char NameAndVersion[] = "V1.1";

extern char *toTimeStr(char *buf, long timeval, char separator = ':');
extern long addToTime(short delta, long timeval, long minval = 1, long maxval = (24*(long)3600)-1);

class Config
{
  public:
    /// <configValues>
    
    /// If adding/removing/changing config items,
    /// then increment version in NameAndVersion string above.
    /// Must also modify body of setDefaults() and getFormattedStr() methods of this class.
    /// It may also be necessary to modify processMenuCommand() method in LcdCountDownTimer file.
    
    char appNameAndVersion[sizeof(NameAndVersion)];

    /// Timer values represented as seconds.
    long timer1ReloadValue;
    long timer2ReloadValue;
    long timer3ReloadValue;
    
    int alarmDuration;       // in seconds
    bool buttonBeep;          // true/false
    byte displayBrightness;   // 1=25%, 2=50%, 3=75%, 4=100%
    int bombArmTime;
    int bombDefuseTime;
    /// </configValues>
    
    char *getFormattedStr(byte cmdId);  // Returns formatted config value associated with menu command id.
    long getTimerReloadValue(byte timerIdx);
    void save();                        // Saves config to EEPROM.
    void load();                        // Loads config from EEPROM.
    void setDefaults();                 // Sets config to default values.
    void copyTo(Config *dest);          // Copies current instance to destination instance.
};

#endif

