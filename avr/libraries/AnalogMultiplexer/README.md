# CMOS 40xx Analog Multiplexer Series Library
A library to interface with analog multiplexers. Supports 4051, 4052, 4053, and
4067 series multiplexers.

## How to use
```
#include <AnalogMultiplexer.h>

enum {
	PIN_EN = 2, // The enable pin of the multiplexer.
	PIN_S0 = 3, // Channel selector pin 0.
	PIN_S1 = 4, // Channel selector pin 1.
	PIN_S2 = 5, // Channel selector pin 2.
	PIN_IO = A0 // Use here a pin that can work in analog & digital modes.
};

AMxx4051 mux{ PIN_EN, PIN_S0, PIN_S1, PIN_S2 }; // 8:1 Multiplexer.

void setup() {
	Serial.begin(9600);
	while (!Serial);
}

void loop() {
	mux.pinMode(PIN_IO, INPUT); // PIN_IO will be used to read from the mux.
	mux.enable(); // Enable multiplexer.
	for (int channel = 0; channel < 8; channel++) {
		int value = mux.analogRead(channel); // Do analog read.
		Serial.println(value);
	}
	Serial.println();
}
```
