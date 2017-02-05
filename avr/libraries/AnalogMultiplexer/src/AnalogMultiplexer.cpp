#include "AnalogMultiplexer.h"

AnalogMultiplexer::AnalogMultiplexer(uint8_t pinEnable) {
	_pinEnable = pinEnable;
	_pinCommon = NOT_A_PIN;
	/*Arduino*/::pinMode(_pinEnable, OUTPUT);
}

void AnalogMultiplexer::enable() {
	/*Arduino*/::digitalWrite(_pinEnable, LOW);
}

void AnalogMultiplexer::disable() {
	/*Arduino*/::digitalWrite(_pinEnable, HIGH);
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
