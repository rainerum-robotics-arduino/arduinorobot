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

#include "TwoWayIntegerEasyTransfer.h"

void CTwoWayIntegerEasyTransfer::begin(Stream * stream) {
	_request.begin(stream);
	_response.begin(stream);
}

void CTwoWayIntegerEasyTransfer::sendSystemReset(void) {
	// Message format: [SYSTEM_RESET]
	writeByte(TwoWayIntegerEasyTransfer.MESSAGE_SYSTEM_RESET);
	sendData();
}

void CTwoWayIntegerEasyTransfer::sendData() {
	_response.sendData();
}

void CTwoWayIntegerEasyTransfer::writeByte(uint8_t data) {
	_response.writeByte(data);
}

void CTwoWayIntegerEasyTransfer::write(uint8_t data) {
	writeByte(data);
}

void CTwoWayIntegerEasyTransfer::writeInt(int16_t data) {
	_response.writeInt(data);
}

void CTwoWayIntegerEasyTransfer::write(int16_t data) {
	writeInt(data);
}

bool CTwoWayIntegerEasyTransfer::hasReceivedData(void) {
	return _request.receiveData();
}

void CTwoWayIntegerEasyTransfer::processInput(void) {
	uint8_t command = _request.readByte();
	switch (command) {
	case MESSAGE_SYSTEM_RESET:
		// Extended system reset callback (0xFF).
		if (_systemResetCallbackFunction != nullptr) {
			_systemResetCallbackFunction();
		}
		break;
	case MESSAGE_FEATURE:
		// Extended feature message callback (0xF0).
		if (_featureMessageCallbackFunction != nullptr) {
			_featureMessageCallbackFunction(_request);
		}
		break;
	default:
		// Default message callback for commands less than 0xF0.
		if (_defaultMessageCallbackFunction != nullptr) {
			_defaultMessageCallbackFunction(command, _request);
		}
		break;
	}
}

void CTwoWayIntegerEasyTransfer::attach(
	DefaultMessageCallbackFunction defaultMessageCallbackFunction
) {
	_defaultMessageCallbackFunction = defaultMessageCallbackFunction;
}

void CTwoWayIntegerEasyTransfer::attach(
	FeatureMessageCallbackFunction featureMessageCallbackFunction
) {
	_featureMessageCallbackFunction = featureMessageCallbackFunction;
}

void CTwoWayIntegerEasyTransfer::attach(
	SystemResetCallbackFunction systemResetCallbackFunction
) {
	_systemResetCallbackFunction = systemResetCallbackFunction;
}

// Make one instance for the user to use.
CTwoWayIntegerEasyTransfer TwoWayIntegerEasyTransfer;
