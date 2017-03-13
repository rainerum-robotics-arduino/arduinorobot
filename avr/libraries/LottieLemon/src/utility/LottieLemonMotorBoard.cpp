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

#ifdef ARDUINO_AVR_ROBOT_MOTOR

using namespace LottieLemon;

static MotorBoard * _motorBoard = nullptr;

MotorBoard::MotorBoard() : _multiplexer{ MUXI, MUXA, MUXB, MUXC } {
	_motorBoard = this;
	TwoWayIntegerEasyTransfer.attach(
		[](uint8_t command, IntegerEasyTransfer & request) {
		if (_motorBoard != nullptr) {
			if (!_motorBoard->handleMessage(command, request)) {
				// TODO: Report unhandled command.
			}
		}
	});
}
/*void RobotMotorBoard::beginIRReceiver(){
IRrecv::enableIRIn();
}*/
void MotorBoard::begin() {
	//init MUX
	_multiplexer.pinMode(MUX_IN, INPUT);
	_multiplexer.enable();

	isPaused = false;
}

void MotorBoard::run() {
	if (isPaused)return;//skip process if the mode is paused

	if (mode == MODE_SIMPLE) {
		//Serial.println("s");
		//do nothing? Simple mode is just about getting commands
	}
	else if (mode == MODE_LINE_FOLLOW) {
		//do line following stuff here.
		LineFollow::runLineFollow();
	}
	else if (mode == MODE_ADJUST_MOTOR) {
		//Serial.println('a');
		//motorAdjustment=analogRead(POT);
		//setSpeed(255,255);
		//delay(100);
	}
}
void MotorBoard::pauseMode(bool onOff) {
	if (onOff) {
		isPaused = true;
	}
	else {
		isPaused = false;
	}
	stopCurrentActions();

}
bool MotorBoard::handleMessage(uint8_t command, IntegerEasyTransfer & request) {
	uint8_t modeName;
	uint8_t codename;
	int value;
	int speedL;
	int speedR;
	uint8_t kP;
	uint8_t kD;
	uint8_t robotSpeedPct;
	uint8_t integrationTimeMillis;
	//Serial.println("data received");
	//Serial.println(command);
	switch (command) {
	case COMMAND_SWITCH_MODE:
		modeName = request.readByte();
		setMode(modeName);
		break;
	case COMMAND_RUN:
		if (mode == MODE_LINE_FOLLOW)
			break;//in follow line mode, the motor does not follow commands
		speedL = request.readInt();
		speedR = request.readInt();
		motorsWrite(speedL, speedR);
		break;
	case COMMAND_MOTORS_STOP:
		motorsStop();
		break;
	case COMMAND_PIN_MODE:
		codename = request.readByte();
		value = request.readInt();
		_pinMode(codename, value);
	case COMMAND_ANALOG_WRITE:
		codename = request.readByte();
		value = request.readInt();
		_analogWrite(codename, value);
		break;
	case COMMAND_DIGITAL_WRITE:
		codename = request.readByte();
		value = request.readByte();
		_digitalWrite(codename, value);
		break;
	case COMMAND_ANALOG_READ:
		codename = request.readByte();
		_analogRead(codename);
		break;
	case COMMAND_DIGITAL_READ:
		codename = request.readByte();
		_digitalRead(codename);
		break;
	case COMMAND_READ_IR:
		_readIR();
		break;
	case COMMAND_READ_TRIM:
		_readTrim();
		break;
	case COMMAND_PAUSE_MODE:
		pauseMode(request.readByte());//onOff state
		break;
	case COMMAND_LINE_FOLLOW_CONFIG:
		kP = request.readByte();
		kD = request.readByte();
		robotSpeedPct = request.readByte();
		integrationTimeMillis = request.readByte();
		LineFollow::config(kP, kD, robotSpeedPct, integrationTimeMillis);
		break;
	default:
		return false;
	}
	return true;
}
uint8_t MotorBoard::parseCodename(uint8_t codename) {
	switch (codename) {
	case B_TK1:
		return TK1;
	case B_TK2:
		return TK2;
	case B_TK3:
		return TK3;
	case B_TK4:
		return TK4;
	default:
		return 0;
	}
}
uint8_t MotorBoard::codenameToAPin(uint8_t codename) {
	switch (codename) {
	case B_TK1:
		return A0;
	case B_TK2:
		return A1;
	case B_TK3:
		return A6;
	case B_TK4:
		return A11;
	default:
		return 0;
	}
}

void MotorBoard::setMode(uint8_t mode) {
	if (mode == MODE_LINE_FOLLOW) {
		LineFollow::calibIRs();
	}
	/*if(mode==SET_MOTOR_ADJUSTMENT){
	save_motor_adjustment_to_EEPROM();
	}
	*/
	/*if(mode==MODE_IR_CONTROL){
	beginIRReceiver();
	}*/
	this->mode = mode;
	//stopCurrentActions();//If line following, this should stop the motors
}

void MotorBoard::stopCurrentActions() {
	motorsStop();
	//motorsWrite(0,0);
}

void MotorBoard::motorsWrite(int speedL, int speedR) {
	/*Serial.print(speedL);
	Serial.print(" ");
	Serial.println(speedR);*/
	//motor adjustment, using percentage
	_refreshMotorAdjustment();

	if (motorAdjustment<0) {
		speedR *= (1 + motorAdjustment);
	}
	else {
		speedL *= (1 - motorAdjustment);
	}

	if (speedR>0) {
		analogWrite(IN_A1, speedR);
		analogWrite(IN_A2, 0);
	}
	else {
		analogWrite(IN_A1, 0);
		analogWrite(IN_A2, -speedR);
	}

	if (speedL>0) {
		analogWrite(IN_B1, speedL);
		analogWrite(IN_B2, 0);
	}
	else {
		analogWrite(IN_B1, 0);
		analogWrite(IN_B2, -speedL);
	}
}
void MotorBoard::motorsWritePct(int speedLpct, int speedRpct) {
	//speedLpct, speedRpct ranges from -100 to 100
	motorsWrite(speedLpct*2.55, speedRpct*2.55);
}
void MotorBoard::motorsStop() {
	analogWrite(IN_A1, 255);
	analogWrite(IN_A2, 255);

	analogWrite(IN_B1, 255);
	analogWrite(IN_B2, 255);
}


/*
 *
 *
 *	Input and Output ports
 *
 *
 */
void MotorBoard::_pinMode(uint8_t codename, int value) {
	uint8_t pin = parseCodename(codename);
	if (pin == 0) { return; } // Not valid codename.
	pinMode(pin, value);
}
void MotorBoard::_digitalWrite(uint8_t codename, bool value) {
	uint8_t pin = parseCodename(codename);
	if (pin == 0) { return; } // Not valid codename.
	digitalWrite(pin, value);
}
void MotorBoard::_analogWrite(uint8_t, int) {
	//There's no PWM available on motor board
}
void MotorBoard::_digitalRead(uint8_t codename) {
	uint8_t pin = parseCodename(codename);
	if (pin == 0) { return; } // Not valid codename.
	bool value = digitalRead(pin);
	TwoWayIntegerEasyTransfer.writeByte(COMMAND_DIGITAL_READ_RE);
	TwoWayIntegerEasyTransfer.writeByte(codename);
	TwoWayIntegerEasyTransfer.writeByte(value);
	TwoWayIntegerEasyTransfer.sendData();
}
void MotorBoard::_analogRead(uint8_t codename) {
	uint8_t pin = codenameToAPin(codename);
	if (pin == 0) { return;	} // Not valid codename.
	int value = analogRead(pin);
	TwoWayIntegerEasyTransfer.writeByte(COMMAND_ANALOG_READ_RE);
	TwoWayIntegerEasyTransfer.writeByte(codename);
	TwoWayIntegerEasyTransfer.writeInt(value);
	TwoWayIntegerEasyTransfer.sendData();
}
int MotorBoard::IRread(uint8_t num) {
	return _IRread(num - 1); //To make consistant with the pins labeled on the board
}

int MotorBoard::_IRread(uint8_t num) {
	if (num < 5) {
		return _multiplexer.analogRead(num);
	}
	return 0;
}


void MotorBoard::_readIR() {
	int value;
	TwoWayIntegerEasyTransfer.writeByte(COMMAND_READ_IR_RE);
	for (int i = 0; i<5; i++) {
		value = _IRread(i);
		TwoWayIntegerEasyTransfer.writeInt(value);
	}
	TwoWayIntegerEasyTransfer.sendData();
}

void MotorBoard::_readTrim() {
	int value = analogRead(TRIM);
	TwoWayIntegerEasyTransfer.writeByte(COMMAND_READ_TRIM_RE);
	TwoWayIntegerEasyTransfer.writeInt(value);
	TwoWayIntegerEasyTransfer.sendData();
}

void MotorBoard::_refreshMotorAdjustment() {
	motorAdjustment = map(analogRead(TRIM), 0, 1023, -30, 30) / 100.0;
}

void MotorBoard::reportActionDone() {
	setMode(MODE_SIMPLE);
	TwoWayIntegerEasyTransfer.writeByte(COMMAND_ACTION_DONE);
	TwoWayIntegerEasyTransfer.sendData();
}

void MotorBoard::reset() {
	begin();
	stopCurrentActions();
}

float MotorBoard::getBatteryTerminalVolts() {
	// Read battery voltage on multiplexer channel 5.
	int adcValue = _multiplexer.analogRead(5);
	float adcVoltage = (5.0f / 1023.0f) * adcValue;
	return 2.0f * adcVoltage; // Compensate for voltage divider.
}

float MotorBoard::getBatteryChargeMilliamps() {
	// Read battery charge current on multiplexer channel 6.
	int adcValue = _multiplexer.analogRead(6);
	float adcVoltage = (5.0f / 1023.0f) * adcValue;
	return adcVoltage / 0.00075f; // Compensate sense resistor and gain.
}

float MotorBoard::getBatteryDischargeMilliamps() {
	// Read battery discharge current on multiplexer channel 7.
	int adcValue = _multiplexer.analogRead(7);
	float adcVoltage = (5.0f / 1023.0f) * adcValue;
	return adcVoltage / 0.00075f; // Compensate sense resistor and gain.
}

#endif // ARDUINO_AVR_ROBOT_MOTOR
