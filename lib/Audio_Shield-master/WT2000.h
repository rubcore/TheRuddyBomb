/* 
Description:
This is a example code for Sandbox Electronics' WT2000 audio play/recording  module.
You can get one of those products on 
http://sandboxelectronics.com

Version:
V0.1

Release Date:
2014-02-16

Author:
Tiequan Shao          info@sandboxelectronics.com

Lisence:
CC BY-NC-SA 3.0

Please keep the above information when you use this code in your project. 
*/


 

#ifndef _WT2000_H_
#define _WT2000_H_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif


#define         WT2000_MAXIMUM_COMMAND_LENGTH           (10)
#define         WT2000_MAXIMUM_REPLY_LENGTH             (10)
#define         WT2000_MAXIMUM_FILE_NAME_LENGTH         (8)

#define         WT2000_COMMAND_PLAY_INDEX               (0xA2)
#define         WT2000_COMMAND_PLAY_NAME                (0xA3)
#define         WT2000_COMMAND_PLAY_INDEX_FOLDER        (0xA4)
#define         WT2000_COMMAND_PLAY_NAME_FOLDER         (0xA5)
#define         WT2000_COMMAND_PAUSE                    (0xAA)
#define         WT2000_COMMAND_STOP                     (0xAB)
#define         WT2000_COMMAND_NEXT                     (0xAC) 
#define         WT2000_COMMAND_PREVIOUS                 (0xAD) 
#define         WT2000_COMMAND_SET_VOLUME               (0xAE)
#define         WT2000_COMMAND_SET_MODE                 (0xAF) 



#define         WT2000_COMMAND_FORWARD                  (0xD0)
#define         WT2000_COMMAND_REWIND                   (0xD1)
#define         WT2000_COMMAND_MEDIA_SELECT             (0XD2) 
#define         WT2000_COMMAND_CHANNEL_SELECT           (0XD3) 
#define         WT2000_COMMAND_SAMPLE_RATE_SELECT       (0XD4) 
#define         WT2000_COMMAND_RECORD_INDEX             (0xD5)
#define         WT2000_COMMAND_RECORD_NAME              (0xD6)
#define         WT2000_COMMAND_RECORD_INDEX_FOLDER      (0xD7)
#define         WT2000_COMMAND_RECORD_NAME_FOLDER       (0xD8)
#define         WT2000_COMMAND_RECORD_STOP              (0xD9)
#define         WT2000_COMMAND_DELETE_INDEX             (0xDA)
#define         WT2000_COMMAND_DELETE_NAME              (0xDB)
#define         WT2000_COMMAND_DELETE_INDEX_FOLDER      (0xDC)
#define         WT2000_COMMAND_DELETE_NAME_FOLDER       (0xDD)
#define         WT2000_COMMAND_DELETE_ALL               (0xDE) 


#define         WT2000_COMMAND_GET_VOLUME               (0xC1)
#define         WT2000_COMMAND_GET_MODE                 (0xC2)
#define         WT2000_COMMAND_FILE_COUNT               (0xC5) 
#define         WT2000_COMMAND_FILE_COUNT_FOLDER        (0xC6) 
#define         WT2000_COMMAND_GET_CURRENT_TRACK        (0xC9) //
#define         WT2000_COMMAND_GET_MEDIA_STATUS         (0xCA)
#define         WT2000_COMMAND_LOOK_UP_FILE             (0xCB)  
#define         WT2000_COMMAND_LOOK_UP_FILE_FOLDER      (0xCC)
#define         WT2000_COMMAND_FREE_SPACE_LEFT          (0xCE)


#define         WT2000_MODE_SINGLE                      (0X00)
#define         WT2000_MODE_SINGLE_CYCLE                (0X01)
#define         WT2000_MODE_ALL_CYCLE                   (0X02)
#define         WT2000_MODE_SHUFFLE                     (0X03)
#define         WT2000_MODE_FOLDER_CYCLE                (0X04)

#define         WT2000_MEDIA_SD                         (0X00)
#define         WT2000_MEDIA_USB                        (0X01)

#define         WT2000_CHANNEL_MIC                      (0X00)
#define         WT2000_CHANNEL_LINE_IN                  (0X01)
#define         WT2000_CHANNEL_AUX                      (0X02)

#define         WT2000_QUALITY_128K                     (0X00)
#define         WT2000_QUALITY_96K                      (0X01)
#define         WT2000_QUALITY_64K                      (0X02)
#define         WT2000_QUALITY_32K                      (0X04)





class WT2000 {
    public:
        WT2000(Stream *myserial);
        uint8_t play(uint16_t index);                  //play by index: 16bit
        uint8_t play(char *name);                      //play by file name: 1-8 characters; ALL LETTERS MUST BE CAPITAL
        uint8_t play(uint16_t index, char *folder);    //play by index in a folder: folder name is fixed to 5 characters wide; ALL LETTERS MUST BE CAPITAL
        uint8_t play(char *file, char *folder);        //play by name in a folder: folder name is fixed to 5 characters wide;ALL LETTERS MUST BE CAPITAL
        uint8_t pause(void);                           //pause or resume current track
        uint8_t stop(void);                            //stop playing current track
        uint8_t volume(void);                          //get current volume (0-32) 0:mute 32:max
        uint8_t volume(uint8_t volume);                //set volume to a specific value(0-32 see above)
        uint8_t forward(void);                         //call to speed up the play forward, call again to resume the play
        uint8_t rewind(void);                          //call to speed up the play backward , call again to resume the play
        uint8_t record(uint16_t index);                //record by index: 16bit
        uint8_t record(char *name);                    //record by file name: 1-8 characters; ALL LETTERS MUST BE CAPITAL
        uint8_t record(uint16_t index, char *folder);  //record by index in a folder: folder name is fixed to 5 characters wide; ALL LETTERS MUST BE CAPITAL
        uint8_t record(char *file, char *folder);      //record by name in a folder: folder name is fixed to 5 characters wide; ALL LETTERS MUST BE CAPITAL
        uint8_t finish(void);                          //stop recording
        uint8_t next(void);                            //play next track
        uint8_t previous(void);                        //play preious track
        uint8_t mode(uint8_t mode);                    //set operating mode: cycle, cycle single, cycle all, shuffle, etc.
        uint8_t mode(void);                            //set operating mode: cycle, cycle single, cycle all, shuffle, etc.
        uint8_t media(uint8_t media);                  //select the storage media
        uint8_t media();                               //get the status of the storage medias(sd card, usb stick)
        uint8_t channel(uint8_t channel);              //select the recording audio source: mic, line-in, aux(stero)
        uint8_t quality(uint8_t quality);              //set the quality of the recording, 128Kbps by default
        
        uint8_t remove(uint16_t index);                //remove file by index: 16bit
        uint8_t remove(char *name);                    //remove file  by file name: 1-8 characters; ALL LETTERS MUST BE CAPITAL
        uint8_t remove(uint16_t index, char *folder);  //remove file  by index in a folder: folder name is fixed to 5 characters wide; ALL LETTERS MUST BE CAPITAL
        uint8_t remove(char *file, char *folder);      //remove file  by name in a folder: folder name is fixed to 5 characters wide; ALL LETTERS MUST BE CAPITAL
        uint8_t erase();                               //erase all file on the storage media
         
        uint16_t count(void);                          //count the number of tracks in the storage media
        uint16_t count(char *folder);                  //count the number of tracks in a spefic folder of the storage media; ALL LETTERS MUST BE CAPITAL
        
        uint8_t lookup(char *file);                    //check if the file is in the root directory, 0:yes, 1:no
        uint8_t lookup(char *file, char *folder);      //check if the file is in the specific folder, 0:yes, 1:no; ALL LETTERS MUST BE CAPITAL
        
        uint16_t room(void);                          //check how many megabytes space left for storage
    private:
		Stream *_myserial;
        void sendcommand(uint8_t opcode, uint8_t data_length, uint8_t *buf);
        uint8_t receivereply(uint8_t data_length, char *buf, uint16_t timeout);
};
#endif
