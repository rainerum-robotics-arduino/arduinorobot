/*
  Copyright (c) 2012 Arduino LLC. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <Arduino.h>
#include <TwoWayIntegerEasyTransfer.h>
#include "../LottieLemon.h"

#ifdef ARDUINO_AVR_ROBOT_CONTROL

using namespace LottieLemon;

static void processRequest() {
	if (TwoWayIntegerEasyTransfer.hasReceivedData()) {
		TwoWayIntegerEasyTransfer.processInput();
	}
}

template<typename Tail>
static void sendData(const Tail & tail) {
	TwoWayIntegerEasyTransfer.writeByte(tail);
	TwoWayIntegerEasyTransfer.sendData();
}

template<typename Tail>
static void requestData(const Tail & tail) {
	sendData(tail);
	processRequest();
}

template<typename Head, typename... Body>
static void sendData(const Head & head, const Body & ... body) {
	TwoWayIntegerEasyTransfer.writeByte(head);
	sendData(body...);
}

template<typename Head, typename... Body>
static void requestData(const Head & head, const Body & ... body) {
	TwoWayIntegerEasyTransfer.writeByte(head);
	requestData(body...);
}

ControlBoard::ControlBoard()
	: _multiplexer{ NOT_A_PIN, MUXA, MUXB, MUXC, MUXD } {
}

void ControlBoard::setMode(uint8_t mode) {
	sendData(COMMAND_SWITCH_MODE, mode);
}

void ControlBoard::pauseMode(bool isPaused) {
	sendData(COMMAND_PAUSE_MODE, isPaused);
}

bool ControlBoard::isActionDone(void) {
	static bool actionValue;
	TwoWayIntegerEasyTransfer.attach(
		[](uint8_t command, IntegerEasyTransfer &) {
		if (command == COMMAND_ACTION_DONE)
			actionValue = true;
		else
			actionValue = false;
	});
	processRequest();
	return actionValue;
}

void ControlBoard::lineFollowConfig(
	uint8_t kP, uint8_t kD,
	uint8_t robotSpeedPercentage, uint8_t intergrationTimeMillis) {
	sendData(COMMAND_LINE_FOLLOW_CONFIG,
		kP, kD, robotSpeedPercentage, intergrationTimeMillis);
}

void ControlBoard::motorsWrite(int speedLeft, int speedRight) {
	sendData(COMMAND_RUN, speedLeft, speedRight);
}

void ControlBoard::motorsStop(void) {
	sendData(COMMAND_MOTORS_STOP);
}

bool ControlBoard::digitalRead(TopMicrocontrollerPin pin) {
	// TODO: set pinMode
	return /*Arduino*/::digitalRead(pin);
}

bool ControlBoard::digitalRead(TopMultiplexerPin pin) {
	int channel = pin - T_TK0;
	// TODO: set pinMode
	_multiplexer.pinMode(MUX_IN, INPUT);
	return _multiplexer.digitalRead(channel);
}

bool ControlBoard::digitalRead(BottomMicrocontrollerPin pin) {
	// TODO: is pinMode set?
	static uint8_t pinCode;
	static uint8_t pinValue;
	pinCode = pin;
	TwoWayIntegerEasyTransfer.attach(
		[](uint8_t command, IntegerEasyTransfer & request) {
		if ((command == COMMAND_DIGITAL_READ_RE) &&
			(request.readByte() == pinCode))
			pinValue = request.readByte();
		else
			pinValue = 0;
	});
	requestData(COMMAND_DIGITAL_READ, pinCode);
	return (pinValue != 0);
}

void ControlBoard::digitalWrite(TopMicrocontrollerPin pin, bool value) {
	// TODO: set pinMode
	/*Arduino*/::digitalWrite(pin, value);
}

void ControlBoard::digitalWrite(BottomMicrocontrollerPin pin, bool value) {
	// TODO: is pinMode set?
	sendData(COMMAND_DIGITAL_WRITE, pin, value);
}

int ControlBoard::analogRead(TopMicrocontrollerPin pin) {
	// TODO: set pinMode
	return /*Arduino*/::analogRead(pin);
}

int ControlBoard::analogRead(TopMultiplexerPin pin) {
	int channel = pin - T_TK0;
	// TODO: set pinMode
	_multiplexer.pinMode(MUX_IN, INPUT);
	return _multiplexer.analogRead(channel);
}

int ControlBoard::analogRead(BottomMicrocontrollerPin pin) {
	// TODO: is pinMode set?
	static uint8_t pinCode = pin;
	static int pinValue;
	pinCode = pin;
	TwoWayIntegerEasyTransfer.attach(
		[](uint8_t command, IntegerEasyTransfer & request) {
		if ((command == COMMAND_ANALOG_READ_RE) &&
			(request.readByte() == pinCode))
			pinValue = request.readInt();
		else
			pinValue = 0;
	});
	requestData(COMMAND_ANALOG_READ, pinCode);
	return pinValue;
}

void ControlBoard::analogWrite(TopMicrocontrollerPin pin, uint8_t value) {
	// TODO: set pinMode
	if (pin == T_TKD4)
		/*Arduino*/::analogWrite(pin, value);
}

uint8_t ControlBoard::updateIR(uint16_t * ir, uint8_t size) {
	static const uint8_t MAX_IR_DATA = 5;
	static uint16_t irValues[MAX_IR_DATA];
	static uint8_t maxItems;
	maxItems = min(MAX_IR_DATA, size);
	TwoWayIntegerEasyTransfer.attach(
		[](uint8_t command, IntegerEasyTransfer & request) {
		for (uint16_t i = 0; i < MAX_IR_DATA; i++) {
			if (command == COMMAND_READ_IR_RE)
				irValues[i] = request.readInt();
			else
				irValues[i] = 0;
		}
	});
	requestData(COMMAND_READ_IR);
	memcpy(ir, irValues, maxItems * sizeof(uint16_t));
	return maxItems;
}

int LottieLemon::ControlBoard::trimRead() {
	static int trimmerValue;
	TwoWayIntegerEasyTransfer.attach(
		[](uint8_t command, IntegerEasyTransfer & request) {
		if (command == COMMAND_READ_TRIM_RE)
			trimmerValue = request.readInt();
		else
			trimmerValue = 0;
	});
	requestData(COMMAND_READ_TRIM);
	return trimmerValue;
}

int ControlBoard::knobRead() {
	return /*Arduino*/::analogRead(POT);
}

#endif // ARDUINO_AVR_ROBOT_CONTROL
