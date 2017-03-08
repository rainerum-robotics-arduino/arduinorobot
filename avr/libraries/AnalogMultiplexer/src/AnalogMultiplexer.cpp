/*
 * Library for the CMOS 40xx analog multiplexer series.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Julian Sanin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "AnalogMultiplexer.h"

AnalogMultiplexer::AnalogMultiplexer(uint8_t pinEnable) {
	_pinEnable = pinEnable;
	_pinCommon = NOT_A_PIN;
	if (NOT_A_PIN != _pinEnable) {
		/*Arduino*/::pinMode(_pinEnable, OUTPUT);
	}
}

void AnalogMultiplexer::enable() {
	if (NOT_A_PIN != _pinEnable) {
		/*Arduino*/::digitalWrite(_pinEnable, LOW);
	}
}

void AnalogMultiplexer::disable() {
	if (NOT_A_PIN != _pinEnable) {
		/*Arduino*/::digitalWrite(_pinEnable, HIGH);
	}
}

void AnalogMultiplexer::pinMode(uint8_t pinCommon, uint8_t mode) {
	if (NOT_A_PIN != digitalPinToPort(pinCommon)) {
		_pinCommon = pinCommon;
		/*Arduino*/::pinMode(_pinCommon, mode);
	} else {
		_pinCommon = NOT_A_PIN;
	}
}

void AnalogMultiplexer::digitalWrite(uint8_t channel, uint8_t value) {
	if ((NOT_A_PIN != _pinCommon) && (NOT_A_PIN != selectChannel(channel))) {
		/*Arduino*/::digitalWrite(_pinCommon, value);
	}
}

int AnalogMultiplexer::digitalRead(uint8_t channel) {
	if ((NOT_A_PIN != _pinCommon) && (NOT_A_PIN != selectChannel(channel))) {
		return /*Arduino*/::digitalRead(_pinCommon);
	}
	return LOW;
}

int AnalogMultiplexer::analogRead(uint8_t channel) {
	if ((NOT_A_PIN != _pinCommon) && (NOT_A_PIN != selectChannel(channel))) {
		return /*Arduino*/::analogRead(_pinCommon);
	}
	return 0;
}
