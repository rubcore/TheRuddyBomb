//Playing music
#include <pitches.h>

#define MusicPin 3
#define notes 100

//encoding
//1 = whole note
//2 = half note
//4 = quarter note
//8 = eighth note
//16 = sixteenth note.

//DPN = duration per note

//Funkytown - Lipps Inc.
unsigned int song_tone[] = {NOTE_A4,0,NOTE_A4,0,NOTE_G4,0,NOTE_A4,0,NOTE_E4,0,NOTE_E4,0,NOTE_G4,0,NOTE_D5,0,NOTE_C5,0,NOTE_A4,0,NOTE_A4,0,NOTE_A4,0,NOTE_G4,0,NOTE_A4,0,NOTE_E4,0,NOTE_E4,0,NOTE_G4,0,NOTE_D5,0,NOTE_C5,0,NOTE_A4,0};
char song_duration[] = {8,8,8,8,8,8,4,4,4,4,8,8,8,8,8,8,8,8,8,1,8,8,8,8,8,8,4,4,4,4,8,8,8,8,8,8,8,8,8,4};
unsigned int items = 40;
float DPN = 1000.0;

//tone array
int note_tone[notes];
//duration array
int note_duration[notes];

unsigned int notes_left;
unsigned long music_temp_time;
float f_duration;
unsigned long u_l_duration;

void setup() {
  // put your setup code here, to run once:
  pinMode(MusicPin,OUTPUT);

  //loop over all notes
  for (int i = 0; i < items; ++i){
    note_tone[i] = song_tone[i];
    note_duration[i] = song_duration[i];
  }

  delay(500);

  notes_left = 0; //reset the pointer
  music_temp_time = millis(); //get the time.

  //calculate the first duration
  f_duration = DPN/song_duration[0]; //calculate the next time to change notes.
  u_l_duration = (unsigned int)f_duration; //in milliseconds.
  
  //start the playing
  while(notes_left < items){

  if (note_tone[notes_left] != 0){
    tone(MusicPin,note_tone[notes_left]); // turn on.
  }
  else noTone(MusicPin); //a rest.
  
    //if we reach the time that the note is finished.
    if ((millis() - music_temp_time) >= u_l_duration){
      
      ++notes_left;//go to the next one.
      music_temp_time = millis(); //get the time.

      f_duration = DPN/song_duration[notes_left]; //calculate the next time to change notes.
      u_l_duration = (unsigned int)f_duration; //in milliseconds.
    }
    
  }

  //end with a no-tone
  noTone(MusicPin);
}

void loop() {
  //do nothing once done.
}
