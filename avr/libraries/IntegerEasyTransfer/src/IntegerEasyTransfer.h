/*******************************************************************************
*  IntegerEasyTransfer 1.1.1 library modified by Julian Sanin, sourced from:
*
*  EasyTransfer Arduino Library v2.1
*       details and example sketch: 
*           http://www.billporter.info/easytransfer-arduino-library/
*
*       Brought to you by:
*              Bill Porter
*              www.billporter.info
*
*       Major props to Mathieu Alorent (kumy) for 
*         I2C version and the pretty pictures.
*
*
*  Lib version history
*   1.0  Created
*   1.1  Fixed dumb Copy-paste error in header file
*        Added a keyword file
*   1.5  Forked lib into Software and Hardware Serial branches, I don't know a
*        better way added passing in of Serial port of different types
*   1.6  Fixed bug where it wasn't clearing out the buffers if the CheckSum
*        failed, I'm good at dumb mistakes
*   1.7  Fixed a bug where the receive function could block for too long and
*        never process data correctly
*        Organized the examples to be Arduino IDE compatible
*   1.8
*        Now Arduino 1.0 compatible!
*   1.81
*        Made it more cross compatible. Man, They really made us work for this
*        one. 
*   2.0
*        Combined SoftEasyTransfer with the other two to make everything one
*        repo
*        Added EasyTransferVirtualWire library for use with Virtual Wire and
*        cheap radios.
*   2.0.1
*        VirtualWire version tested by garth@netram, bugs fixed. 
*   2.1
*        Changes RX parsing buffer to dynamic allocation to conserve RAM.
*   1.0.0
*        Imported EasyTransfer library as IntegerEasyTransfer.
*        Backported ArduinoRobot modifications and restarted version numbering.
*        The library supports a maximum of 20 uint8_t or 10 int16_t values.
*        Mixed uint8_t and int16_t are allowed but care should be taken that the
*        values do not overflow the data buffer.
*   1.1.0
*        Added new classes to support two way communications and extended
*        messages and software reset.
*   1.1.1
*        Added overloaded write method to TwoWayIntegerEasyTransfer.
*        Initalized RX packet variables to zero.
*
*
*  Limits of the Library
*       You can change the Serial port,
*       but the Struct size must not pass 255 bytes
*       VirtualWire Version Struct can'e be bigger then 26 bytes
*
*   The protcol is as follows:
*       Header(0x06,0x85),SizeofPayload,Payload,Checksum
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
#ifndef INTEGER_EASY_TRANSFER_H
#define INTEGER_EASY_TRANSFER_H

#include <Arduino.h>
#include <Stream.h>

class IntegerEasyTransfer {

	enum { MAX_DATA_SIZE = 20 };

public:
	/// <summary>
	/// Start protocol with given transport stream.
	/// </summary>
	/// <param name="theStream">
	/// The reference to the transport stream implementation.
	/// </param>
	void begin(Stream *theStream);

	/// <summary>
	/// Sends out data in binary, with header, length info and checksum.
	/// </summary>
	void sendData();

	/// <summary>
	/// Check if data has been received from the transport stream.
	/// </summary>
	/// <returns>
	/// true if data has been received otherwise false.
	/// </returns>
	bool receiveData();

	/// <summary>
	/// Adds a byte to the protocol buffer. See also:
	/// <seealso cref="sendData" />
	/// </summary>
	/// <param name="data">
	/// The byte to be inserted to the buffer.
	/// </param>
	void writeByte(uint8_t data);

	/// <summary>
	/// Adds a integer to the protocol buffer. See also:
	/// <seealso cref="sendData" />
	/// </summary>
	/// <param name="data">
	/// The integer to be inserted to the buffer.
	/// </param>
	void writeInt(int16_t data);

	/// <summary>
	/// Retrives a byte from the protocol buffer. See also:
	/// <seealso cref="receiveData" />
	/// </summary>
	/// <returns>
	/// The byte from the buffer otherwise 0 if an error occurred.
	/// </returns>
	uint8_t readByte();

	/// <summary>
	/// Retrives a integer from the protocol buffer. See also:
	/// <seealso cref="receiveData" />
	/// </summary>
	/// <returns>
	/// The integer from the buffer otherwise 0 if an error occurred.
	/// </returns>
	int16_t readInt();

private:
	void _resetData();

	uint8_t _data[MAX_DATA_SIZE]; //data storage, for both read and send
	uint8_t _position;

	Stream *_stream;
	uint8_t _size;         //size of data in bytes both for read and send
	uint8_t _rx_array_inx; //index for RX parsing buffer
	uint8_t _rx_len;       //RX packet length according to the packet
	uint8_t _calc_CS;      //calculated Chacksum
};

#endif
