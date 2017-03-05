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

#include "utility/LottieLemonMotorBoard.h"

namespace LottieLemon {

	//Command code
	enum {
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
		COMMAND_LINE_FOLLOW_CONFIG = 100
	};


	//component codename
	enum {
		CN_LEFT_MOTOR  = 0,
		CN_RIGHT_MOTOR = 1,
		CN_IR          = 2
	};

	//motor board modes
	enum {
		MODE_SIMPLE       = 0,
		MODE_LINE_FOLLOW  = 1,
		MODE_ADJUST_MOTOR = 2,
		MODE_IR_CONTROL   = 3
	};

	//bottom TKs, just for communication purpose
	enum {
		B_TK1 = 201,
		B_TK2 = 202,
		B_TK3 = 203,
		B_TK4 = 204
	};

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
}

#endif // LOTTIE_LEMON_H
