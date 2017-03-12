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

#ifndef LOTTIE_LEMON_CONTROL_BOARD_H
#define LOTTIE_LEMON_CONTROL_BOARD_H

#include <stdint.h>
#include <AnalogMultiplexer.h>

namespace LottieLemon {

	class ControlBoard {

		AMxx4067 _multiplexer;

	public:
		ControlBoard();
		void setMode(uint8_t mode);
		void pauseMode(bool isPaused);
		bool isActionDone(void);
		void lineFollowConfig(
			uint8_t kP, uint8_t kD,
			uint8_t robotSpeedPercentage, uint8_t intergrationTimeMillis
		);

		void motorsWrite(int speedLeft, int speedRight);
		void motorsStop(void);

		bool digitalRead(TopMicrocontrollerPin pin);
		bool digitalRead(TopMultiplexerPin pin);
		bool digitalRead(BottomMicrocontrollerPin pin);
		void digitalWrite(TopMicrocontrollerPin pin, bool value);
		void digitalWrite(BottomMicrocontrollerPin pin, bool value);
		int analogRead(TopMicrocontrollerPin pin);
		int analogRead(TopMultiplexerPin pin);
		int analogRead(BottomMicrocontrollerPin pin);
		void analogWrite(TopMicrocontrollerPin pin, uint8_t value);

		uint8_t updateIR(uint16_t * /*[out]*/ ir, uint8_t size);
		int trimRead(void);

		int knobRead(void);
	};
}

#endif // LOTTIE_LEMON_CONTROL_BOARD_H
