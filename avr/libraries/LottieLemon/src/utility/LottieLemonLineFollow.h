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

#ifndef LOTTIE_LEMON_LINE_FOLLOW_H
#define LOTTIE_LEMON_LINE_FOLLOW_H

#include <stdint.h>

namespace LottieLemon {

	class LineFollow {
	public:
		LineFollow();

		void runLineFollow();
		void config(uint8_t KP, uint8_t KD,
			uint8_t robotSpeedPct, uint8_t integrationTimeMillis
		);
		void config(uint8_t KP, uint8_t KI, uint8_t KD,
			uint8_t robotSpeedPct, uint8_t integrationTimeMillis
		);
		//These are all pure virtual functions, pure VF needs pure specifier "=0"
		//virtual void motorsWrite(int speedL, int speedR)=0;
		virtual void motorsWritePct(int speedLpct, int speedRpct) = 0;
		virtual void motorsStop() = 0;
	protected:
		virtual int _IRread(uint8_t num) = 0;
		virtual	void reportActionDone() = 0;
		void calibIRs();

	private:
		void doCalibration(int speedPct, unsigned int time);

		uint8_t _KP;
		uint8_t _KI;
		uint8_t _KD;
		uint8_t _robotSpeedPct;
		uint8_t _integrationTimeMillis;
		unsigned long _tStartMillis;

		uint16_t _sensors[5];
		int16_t _integral = 0;
		int16_t _derivative = 0;
		int16_t _lastError = 0;
	};
}

#endif // LOTTIE_LEMON_LINE_FOLLOW_H
