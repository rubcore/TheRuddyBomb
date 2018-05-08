#ifndef GAMEPARAMS_H_
#define GAMEPARAMS_H_
/*
  Open Source Arduino Airsoft Project - Game parameters
 */

//TIME in MILLISECONDS, use u_long.
//Default settings set to:
//10s arm time
//40 second detonation time
//10s defuse
//5 minute game time

//Bomb arm time
#define ARM_time_default 0
#define ARM_time_count 5
#define ARM_time_10s 10000
#define ARM_time_20s 20000
#define ARM_time_30s 30000
#define ARM_time_40s 40000
#define ARM_time_50s 50000

//Bomb trigger time
#define TRIG_time_default 2
#define TRIG_time_count 5
#define TRIG_time_20s 20000
#define TRIG_time_30s 30000
#define TRIG_time_40s 40000
#define TRIG_time_50s 50000
#define TRIG_time_60s 60000

//Bomb defuse time
#define DEFUSE_time_default 0
#define DEFUSE_time_count 5
#define DEFUSE_time_10s 10000
#define DEFUSE_time_20s 20000
#define DEFUSE_time_30s 30000
#define DEFUSE_time_40s 40000
#define DEFUSE_time_50s 50000

//global game time
#define GAME_time_default 3
#define GAME_time_count 9
#define GAME_time_2m 120000
#define GAME_time_3m 180000
#define GAME_time_4m 240000
#define GAME_time_5m 300000
#define GAME_time_6m 360000
#define GAME_time_7m 420000
#define GAME_time_8m 480000
#define GAME_time_9m 540000
#define GAME_time_10m 600000

#endif