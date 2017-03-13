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

#ifndef LOTTIE_LEMON_H
#define LOTTIE_LEMON_H

#include <Arduino.h>

namespace LottieLemon {

	/*
	A message structure will be:
	switch mode (2):
		byte COMMAND_SWITCH_MODE, byte mode
	run (5):
		byte COMMAND_RUN, int speedL, int speedR
	analogWrite (3):
		byte COMMAND_ANALOG_WRITE, byte codename, byte value;
	digitalWrite (3):
		byte COMMAND_DIGITAL_WRITE, byte codename, byte value;
	analogRead (2):
		byte COMMAND_ANALOG_READ, byte codename;
	analogRead _return_ (4):
		byte COMMAND_ANALOG_READ_RE, byte codename, int value;
	digitalRead (2):
		byte COMMAND_DIGITAL_READ, byte codename;
	digitalRead _return_ (4):
		byte COMMAND_DIGITAL_READ_RE, byte codename, int value;
	read IR (1):
		byte COMMAND_READ_IR;
	read IR _return_ (9):
		byte COMMAND_READ_IR_RE, int valueA, int valueB, int valueC, int valueD;


	*/

	//Command code
	enum Command {
		COMMAND_SWITCH_MODE        = 0,
		COMMAND_RUN                = 10,
		COMMAND_MOTORS_STOP        = 11,
		COMMAND_ANALOG_WRITE       = 20,
		COMMAND_DIGITAL_WRITE      = 30,
		COMMAND_ANALOG_READ        = 40,
		COMMAND_ANALOG_READ_RE     = 41,
		COMMAND_DIGITAL_READ       = 50,
		COMMAND_DIGITAL_READ_RE    = 51,
		COMMAND_READ_IR            = 60,
		COMMAND_READ_IR_RE         = 61,
		COMMAND_ACTION_DONE        = 70,
		COMMAND_READ_TRIM          = 80,
		COMMAND_READ_TRIM_RE       = 81,
		COMMAND_PAUSE_MODE         = 90,
		COMMAND_LINE_FOLLOW_CONFIG = 100,
		COMMAND_PIN_MODE           = 110
	};

	//motor board modes
	enum Mode {
		MODE_SIMPLE       = 0,
		MODE_LINE_FOLLOW  = 1,
		MODE_ADJUST_MOTOR = 2,
		MODE_IR_CONTROL   = 3
	};

	//bottom TKs, just for communication purpose
	enum BottomMicrocontrollerPin {
		B_TK1 = 201,
		B_TK2 = 202,
		B_TK3 = 203,
		B_TK4 = 204,
		BOT_D7  = B_TK3,
		BOT_D8  = B_TK4,
		BOT_D9  = B_TK2,
		BOT_D10 = B_TK1
	};

#ifdef ARDUINO_AVR_ROBOT_CONTROL
	//top TKs
	enum TopMultiplexerPin {
		T_TK0 = 100,
		T_TK1 = 101,
		T_TK2 = 102,
		T_TK3 = 103,
		T_TK4 = 104,
		T_TK5 = 105,
		T_TK6 = 106,
		T_TK7 = 107,
		TOP_M0 = T_TK0,
		TOP_M1 = T_TK1,
		TOP_M2 = T_TK2,
		TOP_M3 = T_TK3,
		TOP_M4 = T_TK4,
		TOP_M5 = T_TK5,
		TOP_M6 = T_TK6,
		TOP_M7 = T_TK7
	};

	//top TKDs
	enum TopMicrocontrollerPin {
		T_TKD0 = TKD0,
		T_TKD1 = TKD1,
		T_TKD2 = TKD2,
		T_TKD3 = TKD3,
		T_TKD4 = TKD4, // Shared with MUXA
		T_TKD5 = TKD5, // Shared with MUXC
		T_LED1 = LED1,
		TOP_D0 = T_TKD0,
		TOP_D1 = T_TKD1,
		TOP_D2 = T_TKD2,
		TOP_D3 = T_TKD3,
		TOP_D4 = T_TKD4, // Shared with MUXA
		TOP_D5 = T_TKD5, // Shared with MUXC
		TOP_LED1 = T_LED1
	};
#endif // ARDUINO_AVR_ROBOT_CONTROL
}

#ifdef ARDUINO_AVR_ROBOT_MOTOR
#include "utility/LottieLemonMotorBoard.h"
#endif
#ifdef ARDUINO_AVR_ROBOT_CONTROL
#include "utility/LottieLemonControlBoard.h"
#endif

#endif // LOTTIE_LEMON_H
