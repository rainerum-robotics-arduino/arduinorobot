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

#ifndef ANALOG_MULTIPLEXER_H
#define ANALOG_MULTIPLEXER_H

#include <Arduino.h>

/// <summary>CMOS Logic Analog Multiplexer and Demultiplexer.</summary>
class AnalogMultiplexer {

	/// <summary>Channel enable.</summary>
	uint8_t _pinEnable;

	/// <summary>Channel common input or output.</summary>
	uint8_t _pinCommon;

protected:
	/// <summary>
	/// Select channel (from 0 to MAX) to be connected to common pin.
	/// </summary>
	virtual uint8_t selectChannel(uint8_t channel) = 0;

public:
	explicit AnalogMultiplexer(uint8_t pinEnable);

	/// <summary>Enables the analog multiplexer channels.</summary>
	void enable();

	/// <summary>Disables the analog multiplexer channels.</summary>
	void disable();

	/// <summary>
	/// Configures the specified common channel pin to behave either as an input
	/// or an output.
	/// </summary>
	/// <param name="pinCommon">
	/// The pin number of whose mode you wish to set. The selected pin will be
	/// used for <see cref="AnalogMultiplexer::digitalRead" />,
	/// <see cref="AnalogMultiplexer::digitalWrite" />, and
	/// <see cref="AnalogMultiplexer::analogRead" />.
	/// </param>
	/// <param name="mode">
	/// <see cref="INPUT" />, <see cref="OUTPUT" />, or
	/// <see cref="INPUT_PULLUP" />.
	/// </param>
	void pinMode(uint8_t pinCommon, uint8_t mode);
	
	/// <summary>
	/// Write a <see cref="HIGH" /> or a <see cref="LOW" /> value to a specified
	/// common channel pin.
	/// <seealso cref="AnalogMultiplexer::pinMode" />
	/// </summary>
	/// <param name="channel">
	/// The channel number.
	/// </param>
	/// <param name="value">
	/// <see cref="HIGH" /> or <see cref="LOW" />.
	/// </param>
	void digitalWrite(uint8_t channel, uint8_t value);
	
	/// <summary>
	/// Reads the value from a specified common channel pin, either
	/// <see cref="HIGH" /> or <see cref="LOW" />.
	/// </summary>
	/// <param name="channel">
	/// The channel number of the multiplexer pin you want to read.
	/// </param>
	/// <returns>
	/// <see cref="HIGH" /> or <see cref="LOW" />.
	/// </returns>
	int digitalRead(uint8_t channel);
	
	/// <summary>
	/// Reads the value from the specified common channel pin.
	/// </summary>
	/// <param name="channel">
	/// The channel number of the multiplexer pin you want to read.
	/// </param>
	/// <returns>
	/// 0 to 1023.
	/// </returns>
	int analogRead(uint8_t channel);
};

/// <summary>
/// Generic class for all types of analog mulitplexer with selector pins.
/// </summary>
template<typename... Args>
class AMxx40xx : public AnalogMultiplexer {

	enum { MAX_SELECTORS = sizeof...(Args) };

	/// <summary>2^n selectors for each channel.</summary>
	uint8_t _pinSelectors[MAX_SELECTORS];

	/// <summary>
	/// See <see cref="AnalogMultiplexer::selectChannel" />.
	/// </summary>
	uint8_t selectChannel(uint8_t channel) {
		if ((1 << MAX_SELECTORS) > channel) {
			for (uint8_t i = 0; i < MAX_SELECTORS; i++) {
				const uint8_t pinSelector = _pinSelectors[i];
				/*Arduino*/::digitalWrite(pinSelector, bitRead(channel, i));
			}
			return channel + 1;
		}
		return NOT_A_PIN;
	}

public:
	/// <summary>
	/// Setup an analog multiplexer with given enable pin and selector pins.
	/// </summary>
	AMxx40xx(uint8_t pinEnable, Args... pinSelector)
		: AnalogMultiplexer(pinEnable), _pinSelectors{pinSelector...} {
		for (uint8_t i = 0; i < MAX_SELECTORS; i++) {
			/*Arduino*/::pinMode(_pinSelectors[i], OUTPUT);
		}
	}
};

/// <summary>
/// 4051 Series Single 8:1 Analog Multiplexer.
/// </summary>
using AMxx4051 = AMxx40xx<uint8_t, uint8_t, uint8_t>;

/// <summary>
/// 4052 Series Dual 4:1 Analog Multiplexer.
/// </summary>
using AMxx4052 = AMxx40xx<uint8_t, uint8_t>;

/// <summary>
/// 4053 Series Triple 2:1 Analog Multiplexer. Assuming all 3 selectors are tied
/// together.
/// </summary>
using AMxx4053 = AMxx40xx<uint8_t>;

/// <summary>
/// 4067 Series Single 16:1 Analog Multiplexer.
/// </summary>
using AMxx4067 = AMxx40xx<uint8_t, uint8_t, uint8_t, uint8_t>;

#endif //ANALOG_MULTIPLEXER
