/*
 * Library for configurable and easy transfers.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Julian Sanin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef TWO_WAY_INTEGER_EASY_TRANSFER_H
#define TWO_WAY_INTEGER_EASY_TRANSFER_H

#include <Arduino.h>
#include "IntegerEasyTransfer.h"

class CTwoWayIntegerEasyTransfer {

	using DefaultMessageCallbackFunction =
		void(*)(uint8_t command, IntegerEasyTransfer & request);
	using FeatureMessageCallbackFunction =
		void(*)(IntegerEasyTransfer & request);
	using SystemResetCallbackFunction = void(*)(void);

	DefaultMessageCallbackFunction _defaultMessageCallbackFunction = nullptr;
	FeatureMessageCallbackFunction _featureMessageCallbackFunction = nullptr;
	SystemResetCallbackFunction _systemResetCallbackFunction = nullptr;

	IntegerEasyTransfer _request;
	IntegerEasyTransfer _response;

public:
	enum {
		/// <summary>Extended message for a feature request.</summary>
		MESSAGE_FEATURE          = 0xF0,
		/// <summary>Extended message for software reset.</summary>
		MESSAGE_SYSTEM_RESET     = 0xF1
	};

	/// <summary>
	/// Start protocol with given transport stream.
	/// </summary>
	/// <param name="stream">
	/// The reference to the transport stream implementation.
	/// </param>
	void begin(Stream * stream);

	/// <summary>Broadcasts a extended system reset message.</summary>
	void sendSystemReset(void);

	/// <summary>
	/// Sends out data in binary, with header, length info and checksum.
	/// For extended feature messages send in following format:
	/// { [MESSAGE_FEATURE]:byte, [EXTENDED_COMMAND]:byte, [PAYLOAD]:byte|int }
	/// For default messages omit the MESSAGE_FEATURE prefix.
	/// </summary>
	void sendData(void);

	/// <summary>
	/// Adds a byte to the protocol buffer. See also:
	/// <seealso cref="sendData" />
	/// </summary>
	/// <param name="data">
	/// The byte to be inserted to the buffer.
	/// </param>
	void writeByte(uint8_t data);

	/// <summary>
	/// Adds a byte to the protocol buffer. See also:
	/// <seealso cref="sendData" />
	/// </summary>
	/// <param name="data">
	/// The byte to be inserted to the buffer.
	/// </param>
	void write(uint8_t data);

	/// <summary>
	/// Adds a integer to the protocol buffer. See also:
	/// <seealso cref="sendData" />
	/// </summary>
	/// <param name="data">
	/// The integer to be inserted to the buffer.
	/// </param>
	void writeInt(int16_t data);

	/// <summary>
	/// Adds a integer to the protocol buffer. See also:
	/// <seealso cref="sendData" />
	/// </summary>
	/// <param name="data">
	/// The integer to be inserted to the buffer.
	/// </param>
	void write(int16_t data);

	/// <summary>Check wether messages have been received.</summary>
	/// <returns>
	/// True if a messages has been recieved otherwise false.
	/// </returns>
	bool hasReceivedData(void);

	/// <summary>Process messages after data has been received.</summary>
	void processInput(void);

	/// <summary>
	/// Connects callback function to listen for default messages.
	/// </summary>
	/// <param name="defaultMessageCallbackFunction">
	/// The default message callback function of type
	/// void(*)(uint8_t command, IntegerEasyTransfer &amp;  request) or nullptr.
	/// </param>
	void attach(DefaultMessageCallbackFunction defaultMessageCallbackFunction);

	/// <summary>
	/// Connects callback function to listen for extended feature messages.
	/// </summary>
	/// <param name="featureMessageCallbackFunction">
	/// The feature message callback function of type
	/// void(*)(IntegerEasyTransfer &amp; request) or a nullptr.
	/// </param>
	void attach(FeatureMessageCallbackFunction featureMessageCallbackFunction);

	/// <summary>
	/// Connects callback function to listen for system reset messages.
	/// </summary>
	/// <param name="systemResetCallbackFunction">
	/// The system reset callback function of type void(*)(void) or a nullptr.
	/// </param>
	void attach(SystemResetCallbackFunction systemResetCallbackFunction);
};

extern CTwoWayIntegerEasyTransfer TwoWayIntegerEasyTransfer;

#endif

