/*******************************************************************************
*  ArduinoRobotEasyTransfer library modified by Julian Sanin,
*  backported from:
*
*  EasyTransfer Arduino Library v2.1
*		details and example sketch: 
*			http://www.billporter.info/easytransfer-arduino-library/
*
*		Brought to you by:
*              Bill Porter
*              www.billporter.info
*
*		Major props to Mathieu Alorent (kumy) for 
*		  I2C version and the pretty pictures.
*
*
*  Lib version history
*	1.0  Created
*	1.1  Fixed dumb Copy-paste error in header file
*		 Added a keyword file
*	1.5  Forked lib into Software and Hardware Serial branches, I don't know a
*        better way added passing in of Serial port of different types
*	1.6  Fixed bug where it wasn't clearing out the buffers if the CheckSum
*        failed, I'm good at dumb mistakes
*	1.7  Fixed a bug where the receive function could block for too long and
*        never process data correctly
*		 Organized the examples to be Arduino IDE compatible
*	1.8
*		 Now Arduino 1.0 compatible!
*   1.81
*		 Made it more cross compatible. Man, They really made us work for this
*        one. 
*   2.0
*        Combined SoftEasyTransfer with the other two to make everything one
*        repo
*        Added EasyTransferVirtualWire library for use with Virtual Wire and
*        cheap radios.
*   2.0.1
*        VirtualWire version tested by garth@netram, bugs fixed. 
*   2.1
*	     Changes RX parsing buffer to dynamic allocation to conserve RAM.
*   3.0
*        Imported EasyTransfer library as ArduinoRobotEasyTransfer.
*        Backported ArduinoRobot modifications.
*
*
*  Limits of the Library
*		You can change the Serial port,
*		but the Struct size must not pass 255 bytes
*		VirtualWire Version Struct can'e be bigger then 26 bytes
*
*   The protcol is as follows:
*		Header(0x06,0x85),SizeofPayload,Payload,Checksum
*	
*  
* This program is free software: you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation, either version 3 of the License, or(at your option) any later
* version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* <http://www.gnu.org/licenses/>
*
* This work is licensed under the Creative Commons Attribution-ShareAlike 3.0
* Unported License. To view a copy of this license, visit
* http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to Creative
* Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*******************************************************************************/
#include "ArduinoRobotEasyTransfer.h"




//Captures address and size of struct
void ArduinoRobotEasyTransfer::begin(uint8_t * ptr, uint8_t length, Stream *theStream){
	address = ptr;
	size = length;
	_stream = theStream;
	
	//dynamic creation of rx parsing buffer in RAM
	rx_buffer = (uint8_t*) malloc(size+1);
}

//Sends out struct in binary, with header, length info and checksum
void ArduinoRobotEasyTransfer::sendData(){
  uint8_t CS = size;
  _stream->write(0x06);
  _stream->write(0x85);
  _stream->write(size);
  for(int i = 0; i<size; i++){
    CS^=*(address+i);
    _stream->write(*(address+i));
  }
  _stream->write(CS);

}

boolean ArduinoRobotEasyTransfer::receiveData(){
  
  //start off by looking for the header bytes. If they were already found in a previous call, skip it.
  if(rx_len == 0){
  //this size check may be redundant due to the size check below, but for now I'll leave it the way it is.
    if(_stream->available() >= 3){
	//this will block until a 0x06 is found or buffer size becomes less then 3.
      while(_stream->read() != 0x06) {
		//This will trash any preamble junk in the serial buffer
		//but we need to make sure there is enough in the buffer to process while we trash the rest
		//if the buffer becomes too empty, we will escape and try again on the next call
		if(_stream->available() < 3)
			return false;
		}
      if (_stream->read() == 0x85){
        rx_len = _stream->read();
		//make sure the binary structs on both Arduinos are the same size.
        if(rx_len != size){
          rx_len = 0;
          return false;
        }
      }
    }
  }
  
  //we get here if we already found the header bytes, the struct size matched what we know, and now we are byte aligned.
  if(rx_len != 0){
    while(_stream->available() && rx_array_inx <= rx_len){
      rx_buffer[rx_array_inx++] = _stream->read();
    }
    
    if(rx_len == (rx_array_inx-1)){
      //seem to have got whole message
      //last uint8_t is CS
      calc_CS = rx_len;
      for (int i = 0; i<rx_len; i++){
        calc_CS^=rx_buffer[i];
      } 
      
      if(calc_CS == rx_buffer[rx_array_inx-1]){//CS good
        memcpy(address,rx_buffer,size);
		rx_len = 0;
		rx_array_inx = 0;
		return true;
		}
		
	  else{
	  //failed checksum, need to clear this out anyway
		rx_len = 0;
		rx_array_inx = 0;
		return false;
	  }
        
    }
  }
  
  return false;
}
