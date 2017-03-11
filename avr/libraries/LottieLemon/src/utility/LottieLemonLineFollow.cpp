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
#include "LottieLemonLineFollow.h"
using namespace LottieLemon;

LineFollow::LineFollow() {
	config(11, 5, 50, 10);
}

void LineFollow::config(uint8_t KP, uint8_t KD,
	uint8_t robotSpeedPct, uint8_t integrationTimeMillis) {
	config(KP, 0, KD, robotSpeedPct, integrationTimeMillis);
}

void LineFollow::config(uint8_t KP, uint8_t KI, uint8_t KD,
	uint8_t robotSpeedPct, uint8_t integrationTimeMillis) {
	_KP = KP;
	_KI = KI;
	_KD = KD;
	_robotSpeedPct = robotSpeedPct;
	_integrationTimeMillis = integrationTimeMillis;
}

void LineFollow::calibIRs() {
	// So only init once.
	static bool isInited = false;
	if (isInited) {
		return;
	}
	delay(1000);
	doCalibration(30, 500);
	doCalibration(-30, 800);
	doCalibration(30, 500);
	delay(1000);
	isInited = true;
}

void LineFollow::runLineFollow() {
	enum { STOP_LINE_VALUE = 1500 };
	uint32_t weight = 0;
	uint16_t sum = 0;
	for (int i = 0; i < 5; i++) {
		_sensors[i] = _IRread(i);
		weight += _sensors[i] * (i + 1);
		sum += _sensors[i];
	}
	if (sum > STOP_LINE_VALUE) {
		if ((millis() - _tStartMillis) <= _integrationTimeMillis) {
			return;
		}
		_tStartMillis = millis();
		int16_t error = 100 * weight / sum - 300;
		_integral = _integral + error;
		_derivative = error - _lastError;
		_lastError = error;
		// Calculate the adjustment with PID control.
		int16_t change = _KP * error + _KI * _integral + _KD * _derivative;
		change = change / 10;
		// Change motor speed.
		int16_t leftSpeed = constrain((_robotSpeedPct + change), -100, 100);
		int16_t rightSpeed = constrain((_robotSpeedPct - change), -100, 100);
		motorsWritePct(leftSpeed, rightSpeed);
	} else {
		// The robot discovered a perpendicular black line. Stop motors.
		motorsStop();
		// Stop the execution of the control board program.
		reportActionDone();
	}
}

void LineFollow::doCalibration(int speedPct, unsigned int time) {
	motorsWritePct(speedPct, -speedPct);
	unsigned long beginTime = millis();
	while ((millis() - beginTime) < time);
	_tStartMillis = millis();
	motorsStop();
}
