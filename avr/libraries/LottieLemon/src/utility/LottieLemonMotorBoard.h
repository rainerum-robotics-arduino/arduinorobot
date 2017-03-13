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

#ifndef LOTTIE_LEMON_MOTOR_BOARD_H
#define LOTTIE_LEMON_MOTOR_BOARD_H

#include <IntegerEasyTransferFeature.h>
#include <AnalogMultiplexer.h>
#include "LottieLemonLineFollow.h"
//#include "IRremote.h"

namespace LottieLemon {

	class MotorBoard : public LineFollow, public IntegerEasyTransferFeature {
	public:
		MotorBoard();
		void begin();

		void run();

		int IRread(uint8_t num);

		float getBatteryTerminalVolts();
		float getBatteryChargeMilliamps();
		float getBatteryDischargeMilliamps();

		void setMode(uint8_t mode);
		void pauseMode(bool onOff);

		void motorsWrite(int speedL, int speedR);
		void motorsWritePct(int speedLpct, int speedRpct);//write motor values in percentage
		void motorsStop();

		bool handleMessage(uint8_t command, IntegerEasyTransfer & request);
		void reset();

	private:
		float motorAdjustment;//-1.0 ~ 1.0, whether left is lowered or right is lowered

							  //convert codename to actual pins
		uint8_t parseCodename(uint8_t codename);
		uint8_t codenameToAPin(uint8_t codename);

		void stopCurrentActions();
		//void sendCommand(byte command,byte codename,int value);

		void _pinMode(uint8_t codename, int value);
		void _analogWrite(uint8_t codename, int value);
		void _digitalWrite(uint8_t codename, bool value);
		void _analogRead(uint8_t codename);
		void _digitalRead(uint8_t codename);
		int _IRread(uint8_t num);
		void _readIR();
		void _readTrim();

		void _refreshMotorAdjustment();

		AMxx4051 _multiplexer;
		uint8_t mode;
		uint8_t isPaused;

		//Line Following 
		void reportActionDone();
	};
}

#endif // LOTTIE_LEMON_MOTOR_BOARD_H
