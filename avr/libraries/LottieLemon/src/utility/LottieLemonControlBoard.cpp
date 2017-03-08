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

static void _requestData(uint8_t command) {
	TwoWayIntegerEasyTransfer.writeByte(command);
	TwoWayIntegerEasyTransfer.sendData();
	if (TwoWayIntegerEasyTransfer.hasReceivedData()) {
		TwoWayIntegerEasyTransfer.processInput();
	}
}

ControlBoard::ControlBoard()
	: _multiplexer{ NOT_A_PIN, MUXA, MUXB, MUXC, MUXD } {
}

void ControlBoard::setMode(uint8_t mode) {
	TwoWayIntegerEasyTransfer.writeByte(COMMAND_SWITCH_MODE);
	TwoWayIntegerEasyTransfer.writeByte(mode);
	TwoWayIntegerEasyTransfer.sendData();
}

void ControlBoard::pauseMode(bool isPaused) {
	TwoWayIntegerEasyTransfer.writeByte(COMMAND_PAUSE_MODE);
	TwoWayIntegerEasyTransfer.writeByte(isPaused);
	TwoWayIntegerEasyTransfer.sendData();
}

bool ControlBoard::isActionDone(void) {
	static bool actionValue;
	TwoWayIntegerEasyTransfer.attach(
		[](uint8_t command, IntegerEasyTransfer &) {
		if (command == COMMAND_ACTION_DONE) {
			actionValue = true;
		}
		else {
			actionValue = false;
		}
	});
	if (TwoWayIntegerEasyTransfer.hasReceivedData()) {
		TwoWayIntegerEasyTransfer.processInput();
	}
	return actionValue;
}

void ControlBoard::lineFollowConfig(
	uint8_t kP, uint8_t kD,
	uint8_t robotSpeedPercentage, uint8_t intergrationTimeMillis) {
	TwoWayIntegerEasyTransfer.writeByte(COMMAND_LINE_FOLLOW_CONFIG);
	TwoWayIntegerEasyTransfer.writeByte(kP);
	TwoWayIntegerEasyTransfer.writeByte(kD);
	TwoWayIntegerEasyTransfer.writeByte(robotSpeedPercentage);
	TwoWayIntegerEasyTransfer.writeByte(intergrationTimeMillis);
	TwoWayIntegerEasyTransfer.sendData();
}

void ControlBoard::motorsWrite(int speedLeft, int speedRight) {
	TwoWayIntegerEasyTransfer.writeByte(COMMAND_RUN);
	TwoWayIntegerEasyTransfer.writeInt(speedLeft);
	TwoWayIntegerEasyTransfer.writeInt(speedRight);
	TwoWayIntegerEasyTransfer.sendData();
}

void ControlBoard::motorsStop(void) {
	TwoWayIntegerEasyTransfer.writeByte(COMMAND_MOTORS_STOP);
	TwoWayIntegerEasyTransfer.sendData();
}

bool ControlBoard::digitalRead(uint8_t port) {
	return false;
}

void ControlBoard::digitalWrite(uint8_t port, bool value) {
	/*
	//bottom TKs, just for communication purpose
	enum {
	B_TK1 = 201,
	B_TK2 = 202,
	B_TK3 = 203,
	B_TK4 = 204
	};
	#define D10 B_TK1
	#define D9 B_TK2
	#define D8 B_TK4
	#define D7 B_TK3
	*/
	/*
	Type Code  Label
	D12 [TKD5] (D5) MUXC Not safe to use!
	D6  [TKD4] (D4) MUXA Not safe to use!
	A4  [TKD3] (D3)
	A3  [TKD2] (D2)
	A2  [TKD1] (D1)
	A1  [TKD0] (D0)
	D17 [LED1] (LED)
	Type Code  Label
	A11 [B_TK4] (D8)
	A6  [B_TK3] (D7)
	A1  [B_TK2] (D9)
	A0  [B_TK1] (D10)
	X0..X7 @ mux
	*/
}

int ControlBoard::analogRead(uint8_t port) {
	return 0;
}

void ControlBoard::analogWrite(uint8_t port, uint8_t value) {
}

uint8_t ControlBoard::updateIR(uint16_t * ir, uint8_t size) {
	static const uint8_t MAX_IR_DATA = 5;
	static uint16_t irValues[MAX_IR_DATA];
	static uint8_t maxItems;
	maxItems = min(MAX_IR_DATA, size);
	TwoWayIntegerEasyTransfer.attach(
		[](uint8_t command, IntegerEasyTransfer & request) {
		for (uint16_t i = 0; i < MAX_IR_DATA; i++) {
			if (command == COMMAND_READ_IR_RE) {
				irValues[i] = request.readInt();
			}
			else {
				irValues[i] = 0;
			}
		}
	});
	_requestData(COMMAND_READ_IR);
	memcpy(ir, irValues, maxItems * sizeof(uint16_t));
	return maxItems;
}

int LottieLemon::ControlBoard::trimRead() {
	static int trimmerValue;
	TwoWayIntegerEasyTransfer.attach(
		[](uint8_t command, IntegerEasyTransfer & request) {
		if (command == COMMAND_READ_TRIM_RE) {
			trimmerValue = request.readInt();
		}
		else {
			trimmerValue = 0;
		}
	});
	_requestData(COMMAND_READ_TRIM);
	return trimmerValue;
}

int ControlBoard::knobRead() {
	return /*Arduino*/::analogRead(POT);
}

#endif // ARDUINO_AVR_ROBOT_CONTROL
