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
	TwoWayIntegerEasyTransfer.write(tail);
	TwoWayIntegerEasyTransfer.sendData();
}

template<typename Tail>
static void requestData(const Tail & tail) {
	sendData(tail);
	processRequest();
}

template<typename Head, typename... Body>
static void sendData(const Head & head, const Body & ... body) {
	TwoWayIntegerEasyTransfer.write(head);
	sendData(body...);
}

template<typename Head, typename... Body>
static void requestData(const Head & head, const Body & ... body) {
	TwoWayIntegerEasyTransfer.write(head);
	requestData(body...);
}

ControlBoard::ControlBoard()
	: _multiplexer{ NOT_A_PIN, MUXA, MUXB, MUXC, MUXD } {
	_multiplexer.pinMode(MUX_IN, INPUT);
}

void ControlBoard::setMode(uint8_t mode) {
	sendData(static_cast<uint8_t>(COMMAND_SWITCH_MODE), mode);
}

void ControlBoard::pauseMode(bool isPaused) {
	sendData(
		static_cast<uint8_t>(COMMAND_PAUSE_MODE),
		static_cast<uint8_t>(isPaused)
	);
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
	sendData(static_cast<uint8_t>(COMMAND_LINE_FOLLOW_CONFIG),
		kP, kD, robotSpeedPercentage, intergrationTimeMillis);
}

void ControlBoard::motorsWrite(int speedLeft, int speedRight) {
	sendData(static_cast<uint8_t>(COMMAND_RUN), speedLeft, speedRight);
}

void ControlBoard::motorsStop(void) {
	sendData(static_cast<uint8_t>(COMMAND_MOTORS_STOP));
}

void ControlBoard::pinMode(TopMicrocontrollerPin pin, uint8_t value) {
	/*Arduino*/::pinMode(pin, value);
}

void ControlBoard::pinMode(BottomMicrocontrollerPin pin, uint8_t value) {
	sendData(
		static_cast<uint8_t>(COMMAND_PIN_MODE),
		static_cast<uint8_t>(pin), value
	);
}

bool ControlBoard::digitalRead(TopMicrocontrollerPin pin) {
	return /*Arduino*/::digitalRead(pin);
}

bool ControlBoard::digitalRead(TopMultiplexerPin pin) {
	int channel = pin - T_TK0;
	return _multiplexer.digitalRead(channel);
}

bool ControlBoard::digitalRead(BottomMicrocontrollerPin pin) {
	static uint8_t pinCode;
	static uint8_t pinValue;
	pinCode = static_cast<uint8_t>(pin);
	TwoWayIntegerEasyTransfer.attach(
		[](uint8_t command, IntegerEasyTransfer & request) {
		if ((command == COMMAND_DIGITAL_READ_RE) &&
			(request.readByte() == pinCode))
			pinValue = request.readByte();
		else
			pinValue = 0;
	});
	requestData(static_cast<uint8_t>(COMMAND_DIGITAL_READ), pinCode);
	return (pinValue != 0);
}

void ControlBoard::digitalWrite(TopMicrocontrollerPin pin, uint8_t value) {
	/*Arduino*/::digitalWrite(pin, value);
}

void ControlBoard::digitalWrite(BottomMicrocontrollerPin pin, uint8_t value) {
	sendData(
		static_cast<uint8_t>(COMMAND_DIGITAL_WRITE),
		static_cast<uint8_t>(pin), value
	);
}

int ControlBoard::analogRead(TopMicrocontrollerPin pin) {
	return /*Arduino*/::analogRead(pin);
}

int ControlBoard::analogRead(TopMultiplexerPin pin) {
	int channel = pin - T_TK0;
	return _multiplexer.analogRead(channel);
}

int ControlBoard::analogRead(BottomMicrocontrollerPin pin) {
	static uint8_t pinCode;
	static int pinValue;
	pinCode = static_cast<uint8_t>(pin);
	TwoWayIntegerEasyTransfer.attach(
		[](uint8_t command, IntegerEasyTransfer & request) {
		if ((command == COMMAND_ANALOG_READ_RE) &&
			(request.readByte() == pinCode))
			pinValue = request.readInt();
		else
			pinValue = 0;
	});
	requestData(static_cast<uint8_t>(COMMAND_ANALOG_READ), pinCode);
	return pinValue;
}

void ControlBoard::analogWrite(TopMicrocontrollerPin pin, uint8_t value) {
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
	requestData(static_cast<uint8_t>(COMMAND_READ_IR));
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
	requestData(static_cast<uint8_t>(COMMAND_READ_TRIM));
	return trimmerValue;
}

int ControlBoard::knobRead() {
	return /*Arduino*/::analogRead(POT);
}

#endif // ARDUINO_AVR_ROBOT_CONTROL
