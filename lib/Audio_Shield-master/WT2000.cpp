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


 





#include <string.h>
#include <WT2000.h>





WT2000::WT2000(Stream *myserial)
{
	_myserial = myserial;
	
}

uint8_t WT2000::quality(uint8_t quality)
{
    sendcommand(WT2000_COMMAND_SAMPLE_RATE_SELECT,4, &quality); 
    
    return receivereply(1,(char *)0, 1000);   
}

uint8_t WT2000::channel(uint8_t channel)
{
    sendcommand(WT2000_COMMAND_CHANNEL_SELECT,4, &channel); 
    
    return receivereply(1,(char *)0, 1000);   
}

uint8_t WT2000::media()
{
    uint8_t media=0;
    sendcommand(WT2000_COMMAND_GET_MEDIA_STATUS,3, (uint8_t *)0); 
    receivereply(2,(char*)&media, 10000);
    return media;
}

uint8_t WT2000::erase()
{

    sendcommand(WT2000_COMMAND_DELETE_ALL,3, (uint8_t *)0); 
    return receivereply(1,(char *)0, 1000);
    
}

uint8_t WT2000::media(uint8_t media)
{
    sendcommand(WT2000_COMMAND_MEDIA_SELECT,4, &media); 
    
    return receivereply(1,(char *)0, 1000);   
}

uint8_t WT2000::mode()
{
    uint8_t mode=0;
    sendcommand(WT2000_COMMAND_GET_MODE,3, (uint8_t *)0); 
    receivereply(2,(char*)&mode, 10000);
    return mode;
}

uint8_t WT2000::mode(uint8_t mode)
{
    sendcommand(WT2000_COMMAND_SET_MODE,4, &mode); 
    
    return receivereply(1,(char *)0, 1000);   
}

uint8_t  WT2000::next(void)
{
    sendcommand(WT2000_COMMAND_NEXT,3, (uint8_t *)0);  
    return receivereply(1,(char *)0, 1000);
}
uint8_t  WT2000::previous(void)
{
    sendcommand(WT2000_COMMAND_PREVIOUS,3, (uint8_t *)0);  
    return receivereply(1,(char *)0, 1000);
}


uint8_t WT2000::play(uint16_t index)
{
    uint16_t index_swap;
    
    index_swap = ((index&0xFF00)>>8);
    index_swap |= ((index&0x00FF)<<8);
    sendcommand(WT2000_COMMAND_PLAY_INDEX,5, (uint8_t *)&index_swap); 
    return receivereply(1,(char *)0, 1000);   
}

uint8_t WT2000::play(uint16_t index, char *folder)
{

    uint8_t buffer[7];
    uint8_t i;
    
    for (i=0;i<5;i++) {
        buffer[i] = folder[i];
    }
    
    buffer[5] = (uint8_t)((index&0xFF00)>>8);
    buffer[6] = (uint8_t)(index&0x00FF);
  
    sendcommand(WT2000_COMMAND_PLAY_INDEX_FOLDER,10, buffer); 
    return receivereply(1,(char *)0, 1000);   
}


uint8_t WT2000::lookup(char *file)
{
    uint8_t name_length;
    name_length = strlen(file);
    name_length %= (WT2000_MAXIMUM_FILE_NAME_LENGTH+1);
    
    sendcommand(WT2000_COMMAND_LOOK_UP_FILE,name_length+3,(uint8_t*)file); 
    return receivereply(1,(char *)0, 1000);   
}

uint8_t WT2000::lookup(char *file, char *folder)
{

    uint8_t buffer[9];
    uint8_t i;
    uint8_t name_length;
    name_length = strlen(file);
    name_length %= (WT2000_MAXIMUM_FILE_NAME_LENGTH+1);
    
    for (i=0;i<5;i++) {
        buffer[i] = folder[i];
    }
    
    for (i=0;i<name_length;i++) {
        buffer[5+i] = file[i];
    }
    sendcommand(WT2000_COMMAND_LOOK_UP_FILE_FOLDER,8+name_length, buffer); 
    return receivereply(1,(char *)0, 1000);   
}

uint8_t WT2000::play(char *file)
{
    uint8_t name_length;
    name_length = strlen(file);
    name_length %= (WT2000_MAXIMUM_FILE_NAME_LENGTH+1);
    
    sendcommand(WT2000_COMMAND_PLAY_NAME,name_length+3,(uint8_t*)file); 
    return receivereply(1,(char *)0, 1000);   
}

uint8_t WT2000::play(char *file, char *folder)
{

    uint8_t buffer[9];
    uint8_t i;
    uint8_t name_length;
    name_length = strlen(file);
    name_length %= (WT2000_MAXIMUM_FILE_NAME_LENGTH+1);
    
    for (i=0;i<5;i++) {
        buffer[i] = folder[i];
    }
    
    for (i=0;i<name_length;i++) {
        buffer[5+i] = file[i];
    }
    sendcommand(WT2000_COMMAND_PLAY_NAME_FOLDER,8+name_length, buffer); 
    return receivereply(1,(char *)0, 1000);   
}

uint8_t WT2000::pause(void)
{
    sendcommand(WT2000_COMMAND_PAUSE,3, (uint8_t *)0);  
    return receivereply(1,(char *)0, 1000);
}

uint8_t WT2000::stop(void) 
{
    sendcommand(WT2000_COMMAND_STOP,3, (uint8_t *)0);  
    return receivereply(1,(char *)0, 1000);  
}


uint16_t WT2000::room(void)
{

 /*
 
	sendcommand(WT2000_COMMAND_FREE_SPACE_LEFT,3, (uint8_t *)0); 
    receivereply(3,(char *)&str, 1000);
	Serial.println("free space left:");
	Serial.print((uint8_t)str[0], HEX);
	Serial.print(" ");
	Serial.print((uint8_t)str[1],HEX);
	Serial.println(" ");
    str[2]=0;
*/

	uint16_t free_space=0;
	uint8_t swap[2];
	sendcommand(WT2000_COMMAND_FREE_SPACE_LEFT,3, (uint8_t *)0); 
    receivereply(3,(char *)&swap, 10000);
	Serial.println("free space left:");
	Serial.print(swap[0], HEX);
	Serial.print(" ");
	Serial.print(swap[1],HEX);
	Serial.println(" ");
    
	free_space |= swap[1];
	free_space |= ((uint16_t)swap[0] << 8 );
	
    

    return free_space;   
}

uint16_t WT2000::count(void)
{
    uint16_t count=0;
    uint16_t count_swap;
    

    sendcommand(WT2000_COMMAND_FILE_COUNT,3, (uint8_t *)0); 
    receivereply(3,(char *)&count_swap, 1000);
    count = ((count_swap&0xFF00)>>8);
    count |= ((count_swap&0x00FF)<<8);
    
    return count;   
}

uint16_t WT2000::count(char *folder)
{
    uint16_t count=0;
    uint16_t count_swap;
    uint8_t buffer[5];
    sendcommand(WT2000_COMMAND_FILE_COUNT_FOLDER,8, (uint8_t *)folder); 
    receivereply(3,(char *)&count_swap, 1000);
    count = ((count_swap&0xFF00)>>8);
    count |= ((count_swap&0x00FF)<<8);
    
    return count;   
}

uint8_t WT2000::volume(void)
{
    uint8_t volume=0;
    sendcommand(WT2000_COMMAND_GET_VOLUME,3, (uint8_t *)0); 
    receivereply(2,(char *)&volume, 10000);
    return volume;
}



uint8_t WT2000::volume(uint8_t volume)
{
    sendcommand(WT2000_COMMAND_SET_VOLUME,4, &volume); 
    
    return receivereply(1,(char *)0, 1000);   
}

uint8_t WT2000::forward(void)
{
    uint8_t volume=0;
    sendcommand(WT2000_COMMAND_FORWARD,3, (uint8_t *)0); 
    return receivereply(1,(char *)0, 1000);
}

uint8_t WT2000::rewind(void)
{
    uint8_t volume=0;
    sendcommand(WT2000_COMMAND_REWIND,3, (uint8_t *)0); 
    return receivereply(1,(char *)0, 1000);

}

uint8_t WT2000::record(uint16_t index)
{
    uint16_t index_swap;
    
    index_swap = ((index&0xFF00)>>8);
    index_swap |= ((index&0x00FF)<<8);
    sendcommand(WT2000_COMMAND_RECORD_INDEX,5, (uint8_t *)&index_swap); 
    return receivereply(1,(char *)0, 1000);   
}

uint8_t WT2000::record(uint16_t index, char *folder) //name of the folder should be 5-characters,otherwise this function will fill the rest of folder name with '0'
{

    uint8_t buffer[7];
    uint8_t i;
    uint8_t folder_name_length;

    folder_name_length = strlen(folder);
    folder_name_length %= (5+1);
    
    for (i=0;i<5;i++) {
        buffer[i] = folder[i];
    }

    while (folder_name_length<5) {
        buffer[folder_name_length++] = '0';
    }
    
    buffer[5] = (uint8_t)((index&0xFF00)>>8);
    buffer[6] = (uint8_t)(index&0x00FF);
  
    sendcommand(WT2000_COMMAND_RECORD_INDEX_FOLDER,10, buffer); 
    return receivereply(1,(char *)0, 1000);   
}

uint8_t WT2000::record(char *file)
{
    uint8_t name_length;
    name_length = strlen(file);
    name_length %= (WT2000_MAXIMUM_FILE_NAME_LENGTH+1);
    
    sendcommand(WT2000_COMMAND_RECORD_NAME,name_length+3,(uint8_t*)file); 
    return receivereply(1,(char *)0, 1000);   
}

uint8_t WT2000::record(char *file, char *folder) //name of the folder should be 5-characters,otherwise this function will fill the rest of folder name with '0'
{

    uint8_t buffer[9];
    uint8_t i;
    uint8_t name_length;
    uint8_t folder_name_length;
    
    name_length = strlen(file);
    name_length %= (WT2000_MAXIMUM_FILE_NAME_LENGTH+1);
    
    folder_name_length = strlen(folder);
    folder_name_length %= (5+1);
    
    for (i=0;i<5;i++) {
        buffer[i] = folder[i];
    }
    
    while (folder_name_length<5) {
        buffer[folder_name_length++] = '0';
    }
        
    
    for (i=0;i<name_length;i++) {
        buffer[5+i] = file[i];
    }
    sendcommand(WT2000_COMMAND_RECORD_NAME_FOLDER,8+name_length, buffer); 
    return receivereply(1,(char *)0, 1000);   
}

uint8_t WT2000::finish(void)
{
    uint8_t volume=0;
    sendcommand(WT2000_COMMAND_RECORD_STOP,3, (uint8_t *)0); 
    return receivereply(1,(char *)0, 1000);  
}

uint8_t WT2000::remove(uint16_t index)
{
    uint16_t index_swap;
    
    index_swap = ((index&0xFF00)>>8);
    index_swap |= ((index&0x00FF)<<8);
    sendcommand(WT2000_COMMAND_DELETE_INDEX,5, (uint8_t *)&index_swap); 
    return receivereply(1,(char *)0, 1000);   
}

uint8_t WT2000::remove(uint16_t index, char *folder)
{

    uint8_t buffer[7];
    uint8_t i;
    
    for (i=0;i<5;i++) {
        buffer[i] = folder[i];
    }
    
    buffer[5] = (uint8_t)((index&0xFF00)>>8);
    buffer[6] = (uint8_t)(index&0x00FF);
  
    sendcommand(WT2000_COMMAND_DELETE_INDEX_FOLDER,10, buffer); 
    return receivereply(1,(char *)0, 1000);   
}

uint8_t WT2000::remove(char *file)
{
    uint8_t name_length;
    name_length = strlen(file);
    name_length %= (WT2000_MAXIMUM_FILE_NAME_LENGTH+1);
    
    sendcommand(WT2000_COMMAND_DELETE_NAME,name_length+3,(uint8_t*)file); 
    return receivereply(1,(char *)0, 1000);   

 }

uint8_t WT2000::remove(char *file, char *folder)
{

    uint8_t buffer[9];
    uint8_t i;
    uint8_t name_length;
	uint8_t result;
    name_length = strlen(file);
    name_length %= (WT2000_MAXIMUM_FILE_NAME_LENGTH+1);
    
    for (i=0;i<5;i++) {
        buffer[i] = folder[i];
    }
    
    for (i=0;i<name_length;i++) {
        buffer[5+i] = file[i];
    }
    sendcommand(WT2000_COMMAND_DELETE_NAME_FOLDER,8+name_length, buffer); 
    return (receivereply(1,(char *)0, 1000));

}

void WT2000::sendcommand(uint8_t opcode, uint8_t data_length, uint8_t *buf)
{
    uint8_t checksum = 0;
    uint8_t i; 
    
    checksum += data_length;
    checksum += opcode;
    
    _myserial->write(0x7E); //Send start code;
    delay(2);
    _myserial->write(data_length);//Send length;
    delay(2);
    _myserial->write(opcode);//Send operation code;
    delay(2);
    for(i=0;i<data_length-3;i++) {
       // Serial.println("command data sent");
        delay(1);
        _myserial->write(buf[i]);
        checksum += buf[i];
    };
    _myserial->write(checksum);
    _myserial->write(0xEF);
   
    return;
}

uint8_t WT2000::receivereply(uint8_t data_length, char *buf,uint16_t timeout)
{
    char op_code;
    uint8_t count = 0;

    _myserial->setTimeout(timeout);
    if ( _myserial->readBytes(&op_code,1) == 0) {
        return 0xff;
    } else {
        count++;
    }
    if ( count < data_length ) {
        count = _myserial->readBytes(buf,data_length-1) + count;
    };
    return op_code;
}

