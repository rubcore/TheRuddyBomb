
cmake_minimum_required(VERSION 3.8)
project(TheRuddyBomb)

set(CMAKE_CXX_STANDARD 11)


set(SOURCE_FILES RuddyBomb/RuddyBomb.ino RuddyBomb/RuddyBomb.h)
add_library(TheRuddyBomb ${SOURCE_FILES})


include_directories(lib/LiquidCrystal)
include_directories(lib/arduino)
include_directories(lib/LcdCountDownTimer)
include_directories(lib/TimerOne)
